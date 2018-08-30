#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct {
    char name[32];
    int age;
} Student;

int main()
{
    // 打开共享内存
    int shmfd = shm_open("/abc",O_RDONLY,0);
    if(shmfd == -1)
          handle_error("shm_open"); 
        
    // 映射共享内存到地址p
    Student *p;
    p = (Student*)mmap(NULL,sizeof(Student),PROT_READ,MAP_SHARED,shmfd,0);
    if(p == NULL)
          handle_error("mmap");
    // 构造存储的变量
    Student orris;
    // 读取p地址的变量到存储的变量
    memcpy(&orris,p,sizeof(Student));
    // 打印变量信息,如名字和年龄
    printf("name=%s age=%d\n",orris.name,orris.age);
    // 关闭共享内存
    close(shmfd);
    // return 0
    return 0;
}
