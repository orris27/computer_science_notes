#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

// 定义学生的结构体
struct Student {
    char name[4];
    int age;
};
int main(int argc, char * argv[])
{
    // 错误提示命令行
    if(argc != 2)
    {
        fprintf(stderr,"Usage:%s <filename>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    // 打开文件,读写的方式打开+错误处理
    int fd = open(argv[1],O_RDWR);
    if(fd == -1)
        ERR_EXIT("open");
    // 写入内容到文件中(可以用ll argv[1];od -c argv[1]查看)
    // 1. 移动文件指针到能存放5个学生的位置
    lseek(fd,sizeof(struct Student)*5-1,SEEK_SET);
    // 2. 填入""到最后一个位置
    write(fd,"",1);
    // 映射文件到共享内存中并得到指针p+错误处理
    struct Student *p;
    p = (struct Student*)mmap(NULL,sizeof(struct Student)*5,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(p == NULL)
        ERR_EXIT("mmap");
    // (对内存的访问就好像是对文件的访问)`
    // 构造5个学生的对象
    for(int i=0;i<5;++i)
    {
        // 读取p指针中(memcpy)
        printf("name=%s age=%d\n",(p+i)->name,(p+i)->age);
    }
    // 解除映射
    if((munmap(p,sizeof(struct Student)*5)) == -1)
        ERR_EXIT("munmap");
}
