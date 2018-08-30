#include "sem.h"

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf; 
};

/*
 * 创建1个信号量集
 */
int sem_create(key_t key)
{
    // 创建1个信号量集:key=参数key,信号量集个数=1,选项=创建|不允许创建2次|权限+错误处理
    int semid = semget(key,1,IPC_CREAT|IPC_EXCL|0666);
    if(semid == -1)
        ERR_EXIT("semget");
    // 返回信号量集的id
    return semid;
}


/*
 * 打开1个信号量集
 */
int sem_open(key_t key)
{
    // 打开1个信号量集.key指明,其他都写0+错误处理
    int semid = semget(key,0,0);
    if(semid == -1)
        ERR_EXIT("semget");
    // 返回信号量集的id
    return semid;
}

/*
 * 设置信号量集中信号量的值
 */
void sem_setval(int semid,int val)
{
    // 定义第4个参数
    union semun su;
    // 赋值
    su.val = val;
    // 设置信号量集中第一个信号量的计数值+错误处理
    if((semctl(semid,0,SETVAL,su)) == -1)
        ERR_EXIT("semctl");
}

/*
 * 获取信号量集中信号量的值
 */
int sem_getval(int semid)
{
    // 获取信号量集中信号量的值:semid,0,GETVAL,0
    int ret = semctl(semid,0,GETVAL,0);
    if (ret == -1)
        ERR_EXIT("semctl");
    return ret;
}
   

/*
 * 删除信号量集
 */
void sem_delete(int semid)
{
    // 删除信号量集:semid,0,IPC_RID
    if((semctl(semid,0,IPC_RMID,0)) == -1)
        ERR_EXIT("semctl");
    
}

/*
 * 执行P操作
 */
void sem_p(int semid)
{
    // 定义P操作
    struct sembuf sops = {0,-1,0};
    // 执行P操作+错误处理
    if ((semop(semid,&sops,1)) == -1)
        ERR_EXIT("semop");
}

/*
 * 执行V操作
 */
void sem_v(int semid)
{
    // 定义V操作
    struct sembuf sops = {0,+1,0};
    // 执行V操作+错误处理
    if ((semop(semid,&sops,1)) == -1)
        ERR_EXIT("semop");
}
