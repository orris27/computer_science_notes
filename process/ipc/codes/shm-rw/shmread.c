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
    // 打开1个共享内存,key=1234,大小=0,是打开,并且权限=0+错误处理
    int shmid = shmget(1235,0,0);
    if(shmid == -1)
        ERR_EXIT("shmget");
    // 连接到共享内存+错误处理
    Student *p = (Student*)shmat(shmid,NULL,0);
    if(p == (void*)-1)
        ERR_EXIT("shmat");
    // 操作共享内存,模拟指针的操作
    printf("name=%s age=%d\n",p->name,p->age);
    // (可以用sleep来ipcs观察连接数)
    // 脱离共享内存
    if((shmdt(p)) == -1)
        ERR_EXIT("shmdt");
    // return 0
    return 0;
}
