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



void print_usage(int argc,char *argv[])
{
    printf("=========================================\n");
    printf("Usage:\n");
    printf("%s -h\thelp\n",argv[0]);
    printf("%s -c\tcreate\n",argv[0]);
    printf("%s -d\tdelete\n",argv[0]);
    printf("%s -s <val>\tset initial value\n",argv[0]);
    printf("%s -p\tP operation\n",argv[0]);
    printf("%s -v\tV operation\n",argv[0]);
    printf("=========================================\n");
}

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
int main(int argc, char *argv[])
{
    if(argc != 2 && argc != 3)
    {
        print_usage(argc,argv);
    }
    int semid;
    // 定义信号量集的key
    key_t key = ftok(".",'s');
    int opt = getopt(argc,argv,"hcds:pv"); //"n"表示-n且没有值,"t:"表示-t且有值
    if (opt == '?') // 出现新的参数.这里不使用ERR_EXIT是因为出现了新的参数也表示成功
        exit(EXIT_SUCCESS);
    if (opt == -1) // 如果没有命令行参数
        exit(EXIT_SUCCESS);
    switch(opt)
    {
        
        case 'h': // 如果是'n'
            // 打印帮助
            print_usage(argc,argv);
            break;
        case 'c':
            // 创建信号量集
            sem_create(key);
            break;
        case 'd':
            // 删除信号量集
            semid = sem_open(key);
            sem_delete(semid);
            break;
        case 's':
            // 设置信号量集的信号初始值
            semid = sem_open(key);
            sem_setval(semid,atoi(optarg));
            break;
        case 'p':
            // 打开信号量集 
            semid = sem_open(key);
            // P操作
            sem_p(semid);
            // 打印当前的计数值
            printf("current value=%d\n",sem_getval(semid));
            break;
        case 'v':
            // 打开信号量集 
            semid = sem_open(key);
            // V操作
            sem_v(semid);
            // 打印当前的计数值
            printf("current value=%d\n",sem_getval(semid));
            break;
            
    }
    return 0;
}
