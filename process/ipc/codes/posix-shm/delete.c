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
    // 打开1个共享内存
    int shmfd = shm_open("/abc",O_RDWR,0);
    if(shmfd == -1)
        handle_error("shm_open"); 
    // 删除共享内存
    if((shm_unlink("/abc")) == -1)
        handle_error("shm_unlink");
    // return 0
    return 0;
}
