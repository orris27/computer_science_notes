#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>   
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct {
    char name[32];
    int age;
} Student;
int main()
{
    // 创建1个共享内存
    int shmfd = shm_open("/abc",O_RDWR|O_CREAT,0666);
    if(shmfd == -1)
        handle_error("shm_open"); 
    // 修改共享内存的大小

    if((ftruncate(shmfd,sizeof(Student))) == -1)
        handle_error("ftruncate");
    
    // 获取共享内存的信息
    struct stat statbuf;
    if((fstat(shmfd,&statbuf)) == -1)
        handle_error("fstat");
    // 打印共享内存的大小
    printf("size=%ld\n",statbuf.st_size);
    // 打印共享内存的权限
    printf("mode=%o\n",statbuf.st_mode & 07777);
    // 关闭共享内存
    close(shmfd);
    // return 0
    return 0;
}
