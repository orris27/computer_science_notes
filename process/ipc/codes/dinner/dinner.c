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
 * 申请刀叉
 */
void wait_for_forks(int semid, int no)
{
    // 获取左刀叉的编号(=信号的编号)
    int left = no;
    // 获取右刀叉的编号(=信号的编号).0=>1,1=>2,2=>3,3=>4,4=>0
    int right = (no+1) % 5;
    // 构造申请2个刀叉的结构(P操作)
    struct sembuf sops[2] = {
        {left,-1,0}, 
        {right,-1,0}
    };
    // 申请2个刀叉
    if ((semop(semid,sops,2)) == -1)
        ERR_EXIT("semop");
}

/*
 * 归还刀叉
 */
void return_forks(int semid, int no)
{
    // 获取左刀叉的编号(=信号的编号)
    int left = no;
    // 获取右刀叉的编号(=信号的编号)
    int right = (no+1) % 5;
    // 构造归还2个刀叉的结构
    struct sembuf sops[2] = {
        {left,+1,0}, 
        {right,+1,0}
    };
    // 归还2个刀叉
    if ((semop(semid,sops,2)) == -1)
        ERR_EXIT("semop");
}



/*
 * 哲学家就餐
 */
void have_dinner(int semid, int no)
{
    // 生成随机种子
    srand(getpid());
    // while(1)
    while(1)
    {
        // 打印说xx哲学家正在思考
        printf("%d is thinking...\n",no);
        // 睡一会,1-3秒
        sleep(rand()%3+1);
        // 打印说xx哲学家肚子饿了
        printf("%d is hungry...\n",no);
        // 申请刀叉
        wait_for_forks(semid, no);
        // 打印说xx哲学家正在吃东西
        printf("%d is eating...\n",no);
        // 吃东西(睡一会儿,1-3秒)
        sleep(rand()%3+1);
        // 归还刀叉
        return_forks(semid, no);
    }
}

int main()
{
    // (1个信号表示1个叉子处于可用状态)
    // 创建容纳5个信号的信号量集
    int semid = semget(IPC_PRIVATE,5,IPC_CREAT|IPC_EXCL|0666);
    if(semid == -1)
            ERR_EXIT("semget");
    // 初始化信号量集的5个信号量,值为1
    union semun su; // 定义第4个参数
    su.val = 1; // 赋值
    for(int i=0;i<5;++i)
    {
        if((semctl(semid,i,SETVAL,su)) == -1) // 设置信号量集中第一个信号量的计数值+错误处理
            ERR_EXIT("semctl");
    }
    /* 创建4个子进程(包括自己在内就是5个进程)*/
    // 定义编号no
    int no = 0;
    // for(i:0=>5)
    for(int i=0;i<4;++i)
    {
        // 创建1个子进程+错误处理
        pid_t pid;
        if ((pid = fork()) == -1)
              ERR_EXIT("fork");
        if (pid == 0) // 子进程
        {
            // 设置no=i
            no = i+1;
            // break
            break;
        }
    }
    // 哲学家就餐
    have_dinner(semid, no);
    // return 0
    return 0;
}
