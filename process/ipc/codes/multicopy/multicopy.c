#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
// 定义进程数N
#define N 5



int main(int argc, char *argv[])
{
    // 命令行错误提示
    if(argc != 3)
    {
        fprintf(stderr,"Usage:%s <src> <dest>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    // 读取源文件名:argv[1]
    // 读取目标文件名:argv[2]
    // 打开源文件:srcfd
    int srcfd = open(argv[1],O_RDONLY,0666);
    if(srcfd == -1)
          handle_error("open");
    // 获取源文件的大小size
    struct stat statbuf;
    if((fstat(srcfd,&statbuf)) == -1)
          handle_error("fstat");
    int size = (int)statbuf.st_size;
    // 打开目标文件
    int destfd = open(argv[2],O_CREAT|O_RDWR|O_TRUNC,0666);
    if(destfd == -1)
          handle_error("open");
    // 修改目标文件大小
    if((ftruncate(destfd,size)) == -1)
          handle_error("ftruncate");
       
    // 映射目标文件到共享内存区
    char *p;
    p = (char*)mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,destfd,0);
    if(p == MAP_FAILED)
          handle_error("mmap");
        
    // 获取该进程要拷贝的大小:size/N
    int copy_block = size/N;
    // for(i:0=>进程数-1)
    for(int i=0;i<4;++i)
    {
        pid_t pid;
        if ((pid = fork()) == -1)
              handle_error("fork");
        if (pid == 0) // 子进程
        {
            // 确定子进程要拷贝的范围:[i*(size/N)开始的size/N大小
            // 移动源文件的指针到i*(size/N))的位置
            lseek(srcfd,i*copy_block,SEEK_SET);
            // 移动共享内存区的指针到i*(size/N)的位置
            p += i*copy_block;

            // 读取源文件的数据到共享内存区的数据:大小=size/N
            read(srcfd,p,copy_block);
            // 结束映射
            p -= i*copy_block;
            if((munmap(p,size)) == -1)
                  handle_error("munmap");
            // 关闭源文件
            if((close(srcfd)) == -1)
                  handle_error("close");
            // 关闭目标文件
            if((close(destfd)) == -1)
                  handle_error("close");
            // 退出
            return 0;
        }
    }

    // 获取父进程要拷贝的大小:copy_size = size-(size/N)*(N-1)
    int copy_size = size - copy_block*(N-1);
    // 确定父进程要拷贝的范围:[(N-1)*(size/N)开始的copy_size大小
    // 移动源文件的指针到(N-1)*(size/N))的位置
    lseek(srcfd,(N-1)*copy_block,SEEK_SET);
    // 移动共享内存区的指针到(N-1)*(size/N)的位置
    p += (N-1)*copy_block;
    // 拷贝源文件的数据到共享内存的数据:大小=copy_size
    read(srcfd,p,copy_size);
    // 回收尸体
    for(int i=0;i<4;++i)
        wait(NULL);
    // 结束映射
    p -= (N-1)*copy_block;
    if((munmap(p,size)) == -1)
          handle_error("munmap");
    // 关闭源文件
    if((close(srcfd)) == -1)
          handle_error("close");
    // 关闭目标文件
    if((close(destfd)) == -1)
          handle_error("close");
    // 退出:0
    return 0;
}
