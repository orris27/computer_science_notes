## 1. 基于mmap实现父子进程间通信的三种方法
1. 临时文件(`mmap-tmp.c`)
    1. 创建1个临时文件
    2. 设置临时文件的大小,使临时文件可以容纳所有数据(注意点1)
    3. 映射该临时文件到进程的地址空间(必须是`MAP_SHARED`,因为父子其实是靠临时文件来通信的)
    4. 创建1个子进程
    5. 父子进程只要使用对应的进程地址空间就能通信了
2. 匿名映射(只能适用于Linux,而不能使用在其他类UNIX系统)(`mmap-anonymous.c`)
    1. 映射1个匿名映射到地址空间
    2. 创建1个子进程
    3. 父子进程只要使用对应的进程地址空间就能通信了
3. "/dev/zero"(适用于所有类UNIX系统)
    1. 打开/dev/zero文件
    2. 映射该临时文件到进程的地址空间(必须是`MAP_SHARED`,因为父子其实是靠临时文件来通信的)
    3. 创建1个子进程
    4. 父子进程只要使用对应的进程地址空间就能通信了
```
./mmap-anonymous
#----------------------------------------------------
# name=orris age=15
#----------------------------------------------------

./mmap-tmp
#----------------------------------------------------
# name=orris age=15
#----------------------------------------------------


./mmap-zero
#----------------------------------------------------
# name=orris age=15
#----------------------------------------------------
```