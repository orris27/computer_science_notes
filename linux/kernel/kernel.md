    
## 1. Linux内核编译
### CentOS
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

make -j4 # 利用.config文件进行编译
make modules_install -j4
make install -j4
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

### Ubuntu(下面的内容在"开始编译"前都是描述如何创建一个可以查询缺页和脏页面的系统调用,但是整个流程是错误的,因为4.8版本内核有问题,只要下载比如说上面的4.13版本或者4.15版本的都可以)
```
uname -r
#------------------------------------------------------------
# 4.15.0-29-generic
#------------------------------------------------------------
uname -m
#------------------------------------------------------------
# x86_64
#------------------------------------------------------------
lsb_release -a
#------------------------------------------------------------
# No LSB modules are available.
# Distributor ID:    Ubuntu
# Description:  Ubuntu 18.04.1 LTS
# Release:  18.04
# Codename:  bionic
#------------------------------------------------------------


mkdir ~/tools
cd ~/tools


sudo apt-get install vim

sudo cp /etc/apt/sources.list /etc/apt/sources.list.bak
sudo vi /etc/apt/sources.list
#################################################################################################
deb http://mirrors.aliyun.com/ubuntu/ bionic main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ bionic main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ bionic-security main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ bionic-security main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ bionic-updates main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ bionic-updates main restricted universe multiverse
##### deb http://mirrors.aliyun.com/ubuntu/ bionic-backports main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ bionic-backports main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ bionic-proposed main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ bionic-proposed main restricted universe multiverse
#################################################################################################

sudo apt-get update # 更新软件列表
sudo apt-get upgrade # 更新软件包

sudo apt-get -y install bc libncurses5-dev libssl-dev make gcc libncurses5-dev libssl-dev libelf-dev

wget http://mirrors.aliyun.com/linux-kernel/v4.x/linux-4.8.tar.xz
wget http://mirrors.aliyun.com/linux-kernel/v4.x/patch-4.8.xz
tar -xvf linux-4.8.tar.xz 
xz -d patch-4.8.xz | patch –p1

cd linux-4.8

# 第一次编译的话，有必要将内核源代码树置于一种完整和一致的状态。因此，我们推荐执行命令make mrproper。它将清除目录下所有配置文件和先前生成核心时产生的.o文件
make mrproper

make menuconfig

sudo vim /usr/include/asm-generic/unistd.h
################################################### 将223处的内容修改成下面的内容
#define __NR_mysyscall  223
__SYSCALL(__NR_mysyscall, sys_mysyscall)
###################################################

vim include/uapi/asm-generic/unistd.h
################################################### 将223处的内容修改成下面的内容
#define __NR_mysyscall  223
__SYSCALL(__NR_mysyscall, sys_mysyscall)
###################################################

vim arch/x86/entry/syscalls/syscall_64.tbl # 我们前面讲过，系统调用处理程序（system_call）会根据eax中的索引到系统调用表（sys_call_table）中去寻找相应的表项。所以，我们必须在那里添加我们自己的一个值。
###################################################
223     common  mysyscall               sys_mysyscall
###################################################

```
修改统计系统缺页次数和进程缺页次数的内核代码

由于每发生一次缺页都要进入缺页中断服务函数do_page_fault一次，所以可以认为执行该函数的次数就是系统发生缺页的次数。可以定义一个全局变量pfcount作为计数变量，在执行do_page_fault时，该变量值加1。在当前进程控制块中定义一个变量pf记录当前进程缺页次数，在执行do_page_fault时，这个变量值加1。

先在`include/linux/mm.h`文件中声明变量pfcount：
```
++ extern unsigned  long  pfcount;
```
要记录进程产生的缺页次数，首先在进程task_struct中增加成员pf01，在`include/linux/sched.h`文件中的task_struct结构中添加pf字段：
```
++ unsigned  long  pf;
```
统计当前进程缺页次数需要在创建进程是需要将进程控制块中的pf设置为0，在进程创建过程中，子进程会把父进程的进程控制块复制一份，实现该复制过程的函数是kernel/fork.c文件中的dup_task_struct()函数，修改该函数将子进程的pf设置成0：
```
    static struct task_struct *dup_task_struct(struct task_struct *orig)
{
        …..
        tsk = alloc_task_struct_node(node);
        if (!tsk)
            return NULL;
       ……
        ++ tsk->pf=0;
        ……
}
```

在arch/x86/mm/fault.c文件中定义变量pfcount；并修改arch/x86/mm/fault.c中do_page_fault()函数。每次产生缺页中断，do_page_fault()函数会被调用，pfcount变量值递增1,记录系统产生缺页次数，current->pf值递增1，记录当前进程产生缺页次数：
```
 ++ unsigned long pfcount;

 __do_page_fault(struct pt_regs *regs, unsigned long error_code)
{
        …
 ++ pfcount++;
 ++ current->pf++;
    …
}
```
sys_mysyscall的实现

我们把这一小段程序添加在kernel/sys.c里面。在这里，我们没有在kernel目录下另外添加自己的一个文件，这样做的目的是为了简单，而且不用修改Makefile，省去不必要的麻烦。

mysyscall系统调用实现输出系统缺页次数、当前进程缺页次数，及每个进程的“脏”页面数: 具体内容参考: https://blog.csdn.net/lishichengyan/article/details/78884082
```
asmlinkage int sys_mysyscall(void)
{
        ……
//printk("当前进程缺页次数：%lu",current->pf)
//每个进程的“脏”页面数
        return 0;
}

#######################我的版本

asmlinkage int sys_mysyscall(void)
{
        struct task_struct* p=NULL;

        printk("<1>total page fault: %lu times\n",pfcount);

        printk("<1>current process's page fault: %lu times\n",current->pf);

        for(p=&init_task;(p=next_task(p))!=&init_task;){

                printk("<1>dirty pages: %d\n pages",p->nr_dirtied);

        }

        return 0;

}


```
接下来的内容都是因为4.8版本内核导致的悲剧,只有make才是有用的,前面都是为了解决各种bug,所以不用参考.但是上面的内容是重要的:开始编译, 解决PIL问题参考:https://unix.stackexchange.com/questions/319761/cannot-compile-kernel-error-kernel-does-not-support-pic-mode/319830
```
##### scripts/config --disable CC_STACKPROTECTOR_STRONG # Special note as of Kernel 4.4 and if compiling using Ubuntu 14.04 (I don't know about 15.10), with an older version of the c compiler: It can not compile with CONFIG_CC_STACKPROTECTOR_STRONG.

##### export ARCH=arm
##### export CROSS_COMPILE=arm-linux-gnueabi-

##### vim Makefile
##### # 解决PIL问题
##### # 我是在799行的地方添加下面内容的
##### #################################################################
##### KBUILD_CFLAGS += $(call cc-option, -fno-pie)
##### KBUILD_CFLAGS += $(call cc-option, -no-pie)
##### KBUILD_AFLAGS += $(call cc-option, -fno-pie)
##### KBUILD_CPPFLAGS += $(call cc-option, -fno-pie) 
##### #################################################################

make -j4

sudo make modules -j4
sudo make modules_install -j4

sudo make install -j4
sudo reboot 
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
