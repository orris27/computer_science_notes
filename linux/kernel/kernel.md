    
## 1. Linux内核编译
1. `make menuconfig`
    1. `<*>`或`[*]`: 将该功能编译到内核中
    2. `[]`: 不将该功能编译到内核中
    3. `[M]`: 允许该功能后期动态地编译到内核中
```
uname -r
#--------------------------------------------
# 3.10.0-862.el7.x86_64
#--------------------------------------------

yum install -y bc


mkdir ~/tools
cd ~/tools

wget http://mirrors.aliyun.com/linux-kernel/v4.x/linux-4.13.3.tar.gz
tar -xvf linux-4.13.3.tar.gz
cd linux-4.13.3
cp /usr/src/linux-headers-4.10.0-38-generic/.config .



#make menuconfig
make

make modules_install
make install
cat /boot/grub/grub.cfg
reboot
uname -r
```



## 2. 编写内核模块,并安装内核模块

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
