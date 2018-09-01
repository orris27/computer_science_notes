#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)


/*
 * SIGALRM处理函数
 */
void handle_sigalrm(int sig)
{
    // 什么都不做
}



/*
 * my_sleep函数
 */
unsigned int my_sleep(unsigned int seconds)
{
    // 定义保存SIGALRM的旧处理函数的变量
    struct sigaction oldact;
    // 注册SIGALRM处理函数,并取出旧的SIGALRM处理函数
    /* 构造sigaction的结构体变量 */
    struct sigaction act;
    /* 设置捕捉函数 */
    act.sa_handler = handle_sigalrm;
    /* 清洗信号集变量 */
    if((sigemptyset(&act.sa_mask)) == -1)
            handle_error("sigemptyset");
    /* 设置捕捉函数处理期间是否屏蔽本信号 */
    act.sa_flags = 0;
    /* 注册SIGINT的的捕捉函数+错误处理 */
    if((sigaction(SIGALRM,&act,&oldact)) == -1)
          handle_error("sigaction");


    // 定义保存旧的阻塞信号集的变量
    sigset_t oldset;

    /* 定义我们的信号集变量 */
    sigset_t my_set;
    /* 清洗信号集变量 */
    if((sigemptyset(&my_set)) == -1)
          handle_error("sigemptyset");
    /* 添加SIGQUIT信号到我们的信号集 */
    if((sigaddset(&my_set,SIGALRM)) == -1)
          handle_error("sigaddset");
    /* 修改屏蔽信号集 */
    if((sigprocmask(SIG_BLOCK,&my_set,&oldset)) == -1)
          handle_error("sigprocmask");

    // 保存旧的阻塞信号集到挂起期间的阻塞信号集
    sigset_t pause_set;
    memcpy(&pause_set,&oldset,sizeof(sigset_t));
    // 删除挂起期间的阻塞信号集中的SIGALRM信号
    if((sigdelset(&pause_set,SIGALRM)) == -1)
          handle_error("sigdelset");
    
    // 开始计时
    alarm(seconds);
    // 解除SIGALRM信号屏蔽(使用挂起期间的阻塞信号集),并且挂起
    sigsuspend(&pause_set);

    // 停止计时,并获得还剩多少时间(防止如果挂起期间被其他信号打断,这时主动关闭计时器)
    unsigned int ret = alarm(0);
    // 恢复SIGALRM的旧处理函数
    if((signal(SIGALRM,oldact.sa_handler)) == SIG_ERR)
          handle_error("signal");
    // 恢复旧的阻塞信号集
    if((sigprocmask(SIG_SETMASK,&oldset,NULL)) == -1)
          handle_error("sigprocmask");
    
    // 返回本来还剩多少时间发出闹钟信号
    return ret;
}


int main()
{
    
    // 睡自己的床
    my_sleep(3);
    // 退出:0
    return 0;
}
