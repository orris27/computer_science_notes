    
## 1. Linux内核编译
[CentOS内核编译参考文档](https://blog.csdn.net/mrzhouxiaofei/article/details/79140435)
1. `make menuconfig`
    1. `<*>`或`[*]`: 将该功能编译到内核中
    2. `[]`: 不将该功能编译到内核中
    3. `[M]`: 允许该功能后期动态地编译到内核中
```
cat /etc/redhat-release 
#--------------------------------------------
# CentOS Linux release 7.5.1804 (Core) 
#--------------------------------------------
uname -r
#--------------------------------------------
# 3.10.0-862.el7.x86_64
#--------------------------------------------
uname -m
#--------------------------------------------
# x86_64
#--------------------------------------------


yum install -y bc ncurses-devel elfutils-libelf-devel openssl-devel wget gcc perl
# yum install -y kernel-devel-$(uname -r) 

mkdir ~/tools
cd ~/tools

wget http://mirrors.aliyun.com/linux-kernel/v4.x/linux-4.13.3.tar.gz
tar -xvf linux-4.13.3.tar.gz
cd linux-4.13.3
#cp /usr/src/linux-headers-4.10.0-38-generic/.config .


##############################################################################
# 确定编译配置: menu界面形式.最后生成.config文件
##############################################################################
#make config #（基于文本的配置界面）
make menuconfig #（基于文本菜单的配置界面）
#make xconfig #（基于图形窗口的配置界面）
#make oldconfig #（基于原来内核配置的基础上修改）

make # 利用.config文件进行编译
make modules_install
make install
#-------------------------------------------------------------
#   ...
#   INSTALL /lib/firmware/edgeport/down2.fw
#   INSTALL /lib/firmware/edgeport/down3.bin
#   INSTALL /lib/firmware/whiteheat_loader.fw
#   INSTALL /lib/firmware/whiteheat.fw
#   INSTALL /lib/firmware/keyspan_pda/keyspan_pda.fw
#   INSTALL /lib/firmware/keyspan_pda/xircom_pgs.fw
#   DEPMOD  4.13.3
# sh ./arch/x86/boot/install.sh 4.13.3 arch/x86/boot/bzImage \
#     System.map "/boot"
#-------------------------------------------------------------


cat /boot/grub/grub.cfg
reboot # 会看到2个内核
uname -r
#--------------------------------------------
# 4.13.3
#--------------------------------------------

```



## 2. 编写内核模块,并安装内核模块
1. `obj-m +=$(TARGET).o`告诉kbuild,希望将`$(TARGET)`,也就是helloworld,编译成内核模块
2. `M=$(PWD)`表示生成的模块文件都将在当前目录下
3. `KDIR`表示源代码最高层目录的位置
```
yum install kernel-devel-$(uname -r)
vim helloworld.c
####################################################################
#define MODULE
#include <linux/module.h>
int init_module(void) 
{
    printk(" Hello World!\n");
    //printk()函数是由Linux内核定义
    return 0; 
}
void cleanup_module(void) 
{
    printk("Goodbye!\n");
}
MODULE_LICENSE("GPL");
####################################################################


####################################################################
# makefile: 注意将4个空格替换成tab键
####################################################################

vim Makefile # 好像小写的makefile是不行的
####################################################################
TARGET = helloworld
KDIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
obj-m += $(TARGET).o
default:
    make -C $(KDIR) M=$(PWD) modules
####################################################################


lsmod | grep helloworld
#-------------------------------------------------------------------------
#-------------------------------------------------------------------------


insmod helloworld.ko 


lsmod | grep helloworld
#-------------------------------------------------------------------------
helloworld             12426  0 
#-------------------------------------------------------------------------


rmmod helloworld
```
