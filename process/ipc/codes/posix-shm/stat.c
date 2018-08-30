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
    // 打开共享内存
    int shmfd = shm_open("/abc",O_RDWR,0);
    if(shmfd == -1)
          handle_error("shm_open"); 
        
    // 获取共享内存信息
    struct stat statbuf;
    if((fstat(shmfd,&statbuf)) == -1)
          handle_error("fstat");

    // 打印共享内存的大小
    printf("size=%ld\n",statbuf.st_size); // 打印共享内存的大小

    // 打印共享内存的权限
    printf("mode=%o\n",statbuf.st_mode & 07777); // 打印共享内存的权限
    // 关闭共享内存
    close(shmfd);
    // return 0
    return 0;
}
