## 1. Makefile
[makefile笔记](https://github.com/orris27/orris/blob/master/cpp/makefile.md).因为github会用tabs8,所以这里把他们换到其他地方了

## 2. 进程
### 2-1. 函数
1. 创建子进程
```
pid_t pid;
pid = fork();
if (pid == 0)
{
    // 子进程处理内容
    exit(EXIT_SUCCESS);
}
else
{
    // 主进程处理内容
}
```

## 3. 库
### 3-1. 静态库
#### 3-1-1. 制作
1. 生成.o文件
2. 打包.o文件
3. 编译时指定静态库
```
tree
#------------------------------------------------------------------------------
# .
# ├── include
# │   └── calc.h
# ├── lib
# ├── main.c
# └── src
#     ├── add.c
#     └── minus.c
#------------------------------------------------------------------------------

vim include/calc.h
###################################################################################
#ifndef _CALC_H_
#define _CALC_H_
int add(int a,int b);
int minus(int a,int b);
#endif
###################################################################################


vim src/add.c
###################################################################################
#include "calc.h"

int add(int a,int b)
{
    return a+b;
}
###################################################################################


vim src/minus.c 
###################################################################################
#include "calc.h"

int minus(int a,int b)
{
    return a-b;
}
###################################################################################

vim main.c 
###################################################################################
#include <stdio.h>
#include "calc.h"

int main()
{
    int a = 1;
    int b = 2;
    printf("%d\n",add(a,b));
    return 0;
}
###################################################################################





cd src/
gcc *.c -c -I ../include
ar rcs libmycalc.a *.o # 
mv libmycalc.a ../lib
cd ..
gcc main.c lib/libmycalc.a -o main -I ./include
#gcc main.c -I ./include -L lib -l mycalc -o main 
./main
```


### 3-2. 动态库
#### 3-2-1. 制作
1. 生成位置无关的.o文件
2. 打包.o文件
3. 编译时指定动态库
```

tree
#------------------------------------------------------------------------------
# .
# ├── include
# │   └── calc.h
# ├── lib
# ├── main.c
# └── src
#     ├── add.c
#     └── minus.c
#------------------------------------------------------------------------------

cd src/
gcc -fPIC -c *.c -I ../include
gcc -shared -o libmycalc.so *.o
mv libmycalc.so ../lib/
cd ..
gcc main.c lib/libmycalc.so -o main -I ../include
#gcc main.c -I ./include -L lib -l mycalc -o main 

./main
#---------------------------------------------------------------------
# ./main: error while loading shared libraries: libmycalc.so: cannot open shared object file: No such file or directory
#---------------------------------------------------------------------

ldd main
#---------------------------------------------------------------------
# 	linux-vdso.so.1 =>  (0x00007ffff1161000)
# 	libmycalc.so => not found # 自己的库没有找到
# 	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fdca53b7000) # 标准c库
# 	/lib64/ld-linux-x86-64.so.2 (0x00007fdca5999000)
#---------------------------------------------------------------------



##########################################################################################
# 方法1:使用系统动态库路径
##########################################################################################

sudo cp lib/libmycalc.so /lib
./main
#---------------------------------------------------------------------
# 3
#---------------------------------------------------------------------
sudo rm -f /lib/libmycalc.so



##########################################################################################
# 方法2:临时/永久自定义动态库路径(修改环境变量)
##########################################################################################
export LD_LIBRARY_PATH=./lib/ # 如果添加到/etc/profile等文件中就能永久生效了
ldd main
#---------------------------------------------------------------------
#	linux-vdso.so.1 =>  (0x00007ffe09b29000)
#	libmycalc.so => ./lib/libmycalc.so (0x00007fae750a9000)
#	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fae74cc9000)
#	/lib64/ld-linux-x86-64.so.2 (0x00007fae754ad000)
#---------------------------------------------------------------------
./main 
#---------------------------------------------------------------------
# 3
#---------------------------------------------------------------------



##########################################################################################
# 方法3:永久自定义动态库路径(修改配置文件)
##########################################################################################
sudo vim /etc/ld.so.conf
########################################################################################
/home/orris/fun/c/test/lib
#########################################################################################

sudo ldconfig -v # 使配置文件生效
./main 
#---------------------------------------------------------------------
# 3
#---------------------------------------------------------------------

```

## 4. UNIX编程
vim下光标放在函数上,用`shift + k`可以跳到对应man文档.按`q`退出
### 4-1. 接口
1. 文件的属性
    1. stat,fstat,lstat:获取属性信息.基本等同于命令行中`stat`命令
        1. stat:不用打开文件也能读文件属性
        2. fstat:需要打开文件才能读文件属性
        3. lstat:不追踪软连接文件.读软连接文件时会只读出软连接文件本身的属性,比如lstat读软连接文件后大小是29字节.其他函数都是读取软连接出去的文件的大小
    2. access:查看文件是否可读,可写,可执行等
    3. chmod,fchmod:修改文件权限
    4. fcntl: f control  
        1. 复制文件描述符.(=dup/dup2)
        2. 获取/修改变已经打开的文件的选项
            1. 使用
                1. `int flag = fcntl(fd,F_GETFL,0);`:获得文件的打开选项
                2. `fcntl(fd,F_SETFL,flag|O_APPEND或者O_NONBLOCK);`
            2. 比如说我以只读方式打开文件后,需要执行追加操作,那么就添加O_APPEND的选项.
            3. 只能设置追加/非阻塞2种模式.只读后再加追加是没有用的.
2. truncate:截取文件长度
    1. 第二个参数表示从该参数位置开始后面的内容都被删除掉
    2. 清空的话,第二个值写0就行了
3. 链接相关
    1. link:创建硬链接
    2. symlink:创建软连接
    3. readlink:读取软连接文件本身的内容(实际上就是链接的内容)
    4. unlink:减少硬链接数,如果减少到0就会删除文件
        1. 实现临时文件的原理:打开1个文件后直接unlink,这样等关闭文件后就会自动删除该文件
4. rename(C库函数)
5. 目录
    1. chdir:修改进程所在的目录.进程默认都是在当前目录下的.
    2. getcwd:获取进程所在的目录
    3. mkdir
    4. rmdir:删除1个空目录
    5. opendir:打开1个目录,必须之后关闭1个目录.返回的是`DIR*`指针,其实类似fopen返回`FILE*`
    6. readdir:读目录.返回`struct dirent*`.指定while循环会自动遍历
    7. closedir:关闭1个目录
6. 文件描述符
    1. dup:复制文件描述符为最小的文件描述符
    2. dup2:复制文件描述符为指定的文件描述符
        1. 如果指定的文件描述符已经被打开了,就先关闭再重定向到要复制的文件描述符
        2. 如果指定的文件描述符=要复制的文件描述符,就什么也不做
    3. 注意:复制文件描述符后,他们都会共同偏移!即先write给fd,再write给fd_copy,后者会在前者后面
7. 环境变量
    1. `extern char** environ;`environ以NULL作为哨兵结尾
        + 保存的格式是"环境变量名=值"
    2. getenv:获得环境变量
    3. setenv:设置环境变量.第三个参数表示是否覆盖原来的环境变量
    4. unsetenv:删除环境变量
8. 用户id
    1. getuid:获得当前终端的用户
    2. geteuid:获得有效用户.比如orris用sudo命令后,`getuid`返回orris,而`geteuid`返回root
    3. getgid
    4. geteuid
#### 4-1-1. 文件
1. 特点
    1. `mode_t mode`:文件的权限.是个八进制的数,实际文件的权限是mode和umask共同处理的结果.详见[Linux权限文档](https://coggle.it/diagram/WzNw5TCAbhFNoY8H/t/linux%E8%A7%92%E8%89%B2)
    2. `int flags`:打开/创建文件的选项.就是O_RDWR,O_CREATE这些.
        1. `O_CREAT|O_EXEL`:查看文件是否存在
        2. `O_TRUNC`:清空文件
    3. `ssize_t`:有符号的`size_t`
    4. `f`开头的函数:一般都是要用文件描述符作为参数,如`fstat`比较于`stat`
2. 属性
    1. `struct stat`中的`st_mode`:保存文件类型(普通文件,目录,块设备,符号连接等)和权限


### 4-2. 结构体
1. 读目录
```
struct dirent {
    ino_t          d_ino;       // 此目录进入点的inode
    off_t          d_off;       // 目录文件开头至此目录进入点的位移
    unsigned short d_reclen;    // d_name的长度,不包含NULL字符
    unsigned char  d_type;      // d_name所指的文件类型(块设备,字符设备,目录DT_DIR,普通文件DT_REG)
    char           d_name[256]; // 文件名
};

```
### 4-3. 错误
1. 我们写的`#include <errno.h>`的文件在`/usr/include/errno.h`里面
    1. `/usr/include/errno.h`里面申明了`extern int errno`,说明errno是个全局变量
2. errno的定义及描述:`/usr/include/asm-generic/errno.h` `/usr/include/asm-generic/errno-base.h`
3. `perror(m)`中的m是用户自定义描述,然后再返回具体的描述


### 4-4. 进程共享
[Process笔记](https://github.com/orris27/orris/blob/master/process/process.md)

## 5. UNIX网络编程
1. [coggle笔记](https://coggle.it/diagram/W33p6eGhIvMJaYJJ/t/socket)
2. [socket笔记](https://github.com/orris27/orris/blob/master/network/socket/socket.md)
3. [socket接口笔记](https://github.com/orris27/orris/blob/master/network/socket/interface.md)
