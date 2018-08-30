#include <fcntl.h>   
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#define handle_error(m) do { perror(m); exit(EXIT_FAILURE); } while(0)


typedef struct{
    char name[32];
    int age;
} Student;

mqd_t mqid;

/*
 * 信号处理函数
 */
void handle_sigusr1(int sig)
{
    // 定义信号事件
    struct sigevent sigev;
    // 填充信号事件
    // 1. 决定信号or线程or不处理
    sigev.sigev_notify = SIGEV_SIGNAL;
    // 2. 如果是信号的话,哪种信号
    sigev.sigev_signo = SIGUSR1;
    // (3. 如果是信号的话,是否要传递参数)
    // 注册消息队列的通知事件
    if((mq_notify(mqid,&sigev)) == -1)
        handle_error("mq_notify");



    // 接收消息
    struct mq_attr attr;
    if((mq_getattr(mqid,&attr)) == -1)
        handle_error("mq_getattr");

    Student stu;
    unsigned int prio;
    if((mq_receive(mqid,(char*)&stu,attr.mq_msgsize,&prio)) == -1)
        handle_error("mq_send");

    // 打印接收到的消息
    printf("prio=%u name=%s age=%d\n",prio,stu.name,stu.age);
}


int main()
{
    // 打开消息队列
    mqid = mq_open("/abc",O_RDONLY);
    if(mqid == (mqd_t) -1)
          handle_error("mq_ope");

    // 注册信号处理函数
    signal(SIGUSR1,handle_sigusr1);
    // 定义信号事件
    struct sigevent sigev;
    // 填充信号事件
    // 1. 决定信号or线程or不处理
    sigev.sigev_notify = SIGEV_SIGNAL;
    // 2. 如果是信号的话,哪种信号
    sigev.sigev_signo = SIGUSR1;
    // (3. 如果是信号的话,是否要传递参数)
    // 注册消息队列的通知事件
    if((mq_notify(mqid,&sigev)) == -1)
        handle_error("mq_notify");

    // while(1)
    while(1)
    {
        // 等待信号(使进程处于等待信号的状态):暂停函数pause();
        pause();
    }
        
    // 解除连接
    if((mq_close(mqid)) == -1)
        handle_error("mq_close");
    // return 0
    return 0;
}
