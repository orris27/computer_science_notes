#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf; 
};

void sem_setval(int semid,int val)
{
    
    union semun su; // 定义第4个参数
    su.val = val; // 赋值
    if((semctl(semid,0,SETVAL,su)) == -1) // 设置信号量集中第一个信号量的计数值+错误处理
        ERR_EXIT("semctl");
}

void sem_p(int semid)
{
    struct sembuf sops = {0,-1,0};
    if ((semop(semid,&sops,1)) == -1)
        ERR_EXIT("semop");
}
void sem_v(int semid)
{
    struct sembuf sops = {0,+1,0};
    if ((semop(semid,&sops,1)) == -1)
        ERR_EXIT("semop");
}

/*
 * 打印某个字符
 */

void print(int semid, char ch)
{
    // while(1)
    while(1)
    {
        // 申请临界区资源
        sem_p(semid);
        // 打印字符
        printf("%c",ch);
        // flush缓冲区
        fflush(stdout);
        // 睡一段时间
        sleep(2);
        // 打印字符
        printf("%c",ch);
        // flush缓冲区
        fflush(stdout);
        // 归还临界区资源
        sem_v(semid);
        // (睡一段时间,防止她又抢到临界区资源)
        sleep(1);
    }
}


int main()
{
    // 创建1个信号量集
    key_t key = ftok(".",'s');
    int semid = semget(key,1,IPC_CREAT|IPC_EXCL|0666);
    if(semid == -1)
          ERR_EXIT("semget");
    // 初始化信号量集,值为1
    sem_setval(semid,1);
    // 创建子进程
    pid_t pid;
    if ((pid = fork()) == -1)
          ERR_EXIT("fork");

    // 如果是子进程
    if (pid == 0)
    {
        // 打印c
        print(semid, 'c');
    }
    // 如果是父进程
    else
    {
        // 打印p
        print(semid, 'p');
    }
    // return 0
    return 0;
}
