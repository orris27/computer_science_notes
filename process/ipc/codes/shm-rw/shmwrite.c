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
    // 创建1个共享内存,key=1234,大小是STU的大小,是创建,并且权限=0666+错误处理
    int shmid = shmget(1235,sizeof(Student),0666|IPC_CREAT);
    if(shmid == -1)
        ERR_EXIT("shmget");
    // 连接到共享内存+错误处理
    Student *p = (Student*)shmat(shmid,NULL,0);
    if(p == (void*)-1)
        ERR_EXIT("shmat");
    // 操作共享内存,模拟指针的操作
    strcpy(p->name,"ab");
    p->age = 15;
    // (可以用sleep来ipcs观察连接数)
    // 脱离共享内存
    if((shmdt(p)) == -1)
        ERR_EXIT("shmdt");
    // return 0
    return 0;
}
