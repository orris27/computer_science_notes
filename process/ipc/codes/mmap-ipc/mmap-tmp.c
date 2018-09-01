#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

// 定义学生的结构体
typedef struct{
    char name[32];
    int age;
} Student;

int main()
{

    // 创建1个临时文件
    /* 创建文件 */
    int fd;
    if((fd = open("tmpfile",O_RDWR|O_CREAT,0666)) == -1)
          handle_error("open");
    /* 减少文件的硬链接数 */
    if((unlink("tmpfile")) == -1)
        handle_error("unlink");
    // 修改临时文件的大小
    if((ftruncate(fd,sizeof(Student))) == -1)
        handle_error("ftruncate");


    // 映射该临时文件到进程的地址空间
    Student *p;
    p = (Student*)mmap(NULL,sizeof(Student),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(p == MAP_FAILED)
          handle_error("mmap");
    // 创建1个子进程
    pid_t pid;
    if ((pid = fork()) == -1)
          handle_error("fork");

    // 如果是子进程的话
    if (pid == 0)
    {
        // 构造学生结构体变量
        Student orris;
        // 填充学生变量
        strcpy(orris.name,"orris");
        orris.age = 15;
        // 写入到地址空间
        memcpy(p,&orris,sizeof(Student)); 
        if((munmap(p,sizeof(Student))) == -1)
              handle_error("munmap");
    }
    // 如果是父进程的话
    else
    {
        // 睡一会儿
        sleep(1);
        // 定义学生结构体变量
        Student stu;
        // 读取地址空间里的学生结构体 
        memcpy(&stu,p,sizeof(Student));  
        // 打印学生信息
        printf("name=%s age=%d\n",stu.name,stu.age);

        if((munmap(p,sizeof(Student)*5)) == -1)
              handle_error("munmap");
    }
    /* 关闭文件 */
    close(fd);
    // 返回0
    return 0;
}
