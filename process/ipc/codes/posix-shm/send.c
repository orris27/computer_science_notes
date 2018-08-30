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
    int shmfd = shm_open("/abc",O_RDWR,0);
    if(shmfd == -1)
          handle_error("shm_open"); 
        
    // 映射共享内存到地址p
    Student *p;
    p = (Student*)mmap(NULL,sizeof(Student),PROT_READ|PROT_WRITE,MAP_SHARED,shmfd,0);
    if(p == NULL)
          handle_error("mmap");
    // 构造存储的变量
    Student orris;
    strcpy(orris.name,"orris");
    orris.age = 15;
    // 写入变量到p地址
    memcpy(p,&orris,sizeof(Student));
    // 关闭共享内存
    close(shmfd);
    // return 0
    return 0;
}
