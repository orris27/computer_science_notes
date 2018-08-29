#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)



typedef struct{
    char name[4];
    int age;
} Student;

int main()
{
    // 打开key=1234的共享内存+错误处理
    int shmid = shmget(1234,0,0);
    if(shmid == -1)
          ERR_EXIT("shmget");
    // 删除共享内存段
    if((shmctl(shmid,IPC_RMID,NULL)) == -1)
          ERR_EXIT("msgctl");
    // return 0
    return 0;
}
