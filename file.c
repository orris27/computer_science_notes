/*
 *  linux/fs/myext2/file.c
 *
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/fs/minix/file.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  myext2 fs regular file handling primitives
 *
 *  64-bit file support on 64-bit platforms by Jakub Jelinek
 * 	(jj@sunsite.ms.mff.cuni.cz)
 */



#include <linux/time.h>
#include <linux/pagemap.h>
#include <linux/quotaops.h>
#include "myext2.h"
#include "xattr.h"
#include "acl.h"
#include <linux/uio.h>

#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/file.h>
#include <linux/uio.h>
#include <linux/aio.h>
#include <linux/fsnotify.h>
#include <linux/security.h>
#include <linux/export.h>
#include <linux/syscalls.h>
#include <linux/pagemap.h>
#include <linux/splice.h>
#include <linux/compat.h>

#include <asm/uaccess.h>
#include <asm/unistd.h>

ssize_t new_sync_read_crypt(struct file *filp, const char __user *buf, size_t len, loff_t *ppos);
ssize_t new_sync_write_crypt(struct file *filp, const char __user *buf, size_t len, loff_t *ppos);



/*
 * Called when filp is released. This happens when all file descriptors
 * for a single struct file are closed. Note that different open() calls
 * for the same file yield different struct file structures.
 */
static int myext2_release_file (struct inode * inode, struct file * filp)
{
	if (filp->f_mode & FMODE_WRITE) {
		mutex_lock(&MYEXT2_I(inode)->truncate_mutex);
		myext2_discard_reservation(inode);
		mutex_unlock(&MYEXT2_I(inode)->truncate_mutex);
	}
	return 0;
}

int myext2_fsync(struct file *file, loff_t start, loff_t end, int datasync)
{
	int ret;
	struct super_block *sb = file->f_mapping->host->i_sb;
	struct address_space *mapping = sb->s_bdev->bd_inode->i_mapping;

	ret = generic_file_fsync(file, start, end, datasync);
	if (ret == -EIO || test_and_clear_bit(AS_EIO, &mapping->flags)) {
		/* We don't really know where the IO error happened... */
		myext2_error(sb, __func__,
			   "detected IO error when writing metadata buffers");
		ret = -EIO;
	}
	return ret;
}

/*
 * We have mostly NULL's here: the current defaults are ok for
 * the myext2 filesystem.
 */
const struct file_operations myext2_file_operations = {
	.llseek		= generic_file_llseek,
	.read		= new_sync_read_crypt,
	.write		= new_sync_write_crypt,
	.read_iter	= generic_file_read_iter,
	.write_iter	= generic_file_write_iter,
	.unlocked_ioctl = myext2_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= myext2_compat_ioctl,
#endif
	.mmap		= generic_file_mmap,
	.open		= dquot_file_open,
	.release	= myext2_release_file,
	.fsync		= myext2_fsync,
	.splice_read	= generic_file_splice_read,
	.splice_write	= iter_file_splice_write,
};

#ifdef CONFIG_MYEXT2_FS_XIP
const struct file_operations myext2_xip_file_operations = {
	.llseek		= generic_file_llseek,
	.read		= xip_file_read,
	.write		= xip_file_write,
	.unlocked_ioctl = myext2_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= myext2_compat_ioctl,
#endif
	.mmap		= xip_file_mmap,
	.open		= dquot_file_open,
	.release	= myext2_release_file,
	.fsync		= myext2_fsync,
};
#endif

const struct inode_operations myext2_file_inode_operations = {
#ifdef CONFIG_MYEXT2_FS_XATTR
	.setxattr	= generic_setxattr,
	.getxattr	= generic_getxattr,
	.listxattr	= myext2_listxattr,
	.removexattr	= generic_removexattr,
#endif
	.setattr	= myext2_setattr,
	.get_acl	= myext2_get_acl,
	.set_acl	= myext2_set_acl,
	.fiemap		= myext2_fiemap,
};

ssize_t new_sync_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos)
{
        struct iovec iov = { .iov_base = buf, .iov_len = len };
        struct kiocb kiocb;
        struct iov_iter iter;
        ssize_t ret;

        init_sync_kiocb(&kiocb, filp);
        kiocb.ki_pos = *ppos;
        kiocb.ki_nbytes = len;
        iov_iter_init(&iter, READ, &iov, 1, len);

        ret = filp->f_op->read_iter(&kiocb, &iter);
        if (-EIOCBQUEUED == ret)
                ret = wait_on_sync_kiocb(&kiocb);
        *ppos = kiocb.ki_pos;
        return ret;
};

ssize_t new_sync_write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos)
{
        struct iovec iov = { .iov_base = (void __user *)buf, .iov_len = len };
        struct kiocb kiocb;
        struct iov_iter iter;
        ssize_t ret;

        init_sync_kiocb(&kiocb, filp);
        kiocb.ki_pos = *ppos;
        kiocb.ki_nbytes = len;
        iov_iter_init(&iter, WRITE, &iov, 1, len);

        ret = filp->f_op->write_iter(&kiocb, &iter);
        if (-EIOCBQUEUED == ret)
                ret = wait_on_sync_kiocb(&kiocb);
        *ppos = kiocb.ki_pos;
        return ret;
};



ssize_t new_sync_read_crypt(struct file *filp, const char __user *buf, size_t len, loff_t *ppos)
{
    int i;
    char *mybuf = buf;
    ssize_t ret = new_sync_read(filp, buf, len, ppos);
    char tmp;
    for(i=0; i<len; i++) {
        get_user(tmp, mybuf+i);
        tmp = tmp - 25;
        if(tmp < 0) tmp = tmp + 128;
        put_user(tmp, mybuf + i);
    }
    printk("haha encrypt %ld\n", len);
    return ret;
};

ssize_t new_sync_write_crypt(struct file *filp, const char __user *buf, size_t len, loff_t *ppos)
{
    int i;
    char *mybuf = buf;
    char tmp;
    for(i=0; i<len; i++) {
        get_user(tmp, mybuf+i);
        tmp = tmp + 25;
        if(tmp >= 128) tmp = tmp - 128;
        put_user(tmp, mybuf + i);
    }
    printk("haha encrypt %ld\n", len);
    return new_sync_write(filp, mybuf, len, ppos);
};

