#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

// 定义计数君
int n = 1;
// 定义可以给对方发信号的标志:初始值=False
int flag = 0;

/*
 * SIGUSR1处理函数
 */
void handle_sigusr1(int sig)
{
    // 打印说子进程当前数到哪里了
    printf("I am a child  %d %d\n",getpid(),n);
    // 更新数值
    n += 2;
    // 说可以给父进程发信号了
    flag = 1; // 解决方法2:直接在这里给对方发信号,而不用全局变量

    /* 解决方法1  */
    //sleep(1); // 给对方足够的时间去拿回CPU时间片并置flag为0
}
/*
 * SIGUSR2处理函数
 */

void handle_sigusr2(int sig)
{
    // 打印说父进程当前数到哪里了
    printf("I am a parent %d %d\n",getpid(),n);
    // 更新数值
    n += 2;
    // 说可以给子进程发信号了
    flag = 1; // 解决方法2:直接在这里给对方发信号,而不用全局变量

    /* 解决方法1  */
    //sleep(1); // 给对方足够的时间去拿回CPU时间片并置flag为0
}



int main()
{
    // 创建1个子进程
    pid_t pid;
    if ((pid = fork()) == -1)
          handle_error("fork");

    // 如果是子进程
    if (pid == 0)
    {
        // 设置计数君为2
        n = 2;
        // 注册SIGUSR1信号的捕捉函数
        if((signal(SIGUSR1,handle_sigusr1)) == SIG_ERR)
              handle_error("signal");
        // while(1)
        while(1)
        {
            // 如果可以给父进程发信号
            if(flag == 1)
            {
                // 发送SIGUSR2信号给父进程
                kill(getppid(),SIGUSR2);
                // 说不可以给父进程发信号了
                flag = 0;
            }
        }
    }
    // 如果是父进程
    else
    {
        // 注册SIGUSR2信号的捕捉函数
        if((signal(SIGUSR2,handle_sigusr2)) == SIG_ERR)
              handle_error("signal");
        // 睡一会儿,等待子进程注册好捕捉函数
        sleep(1);
        // 数数
        handle_sigusr2(0);
        // while(1)
        while(1)
        {
            // 如果可以给子进程发信号
            if(flag == 1)
            {
                // 发送SIGUSR1信号给子进程
                kill(pid,SIGUSR1);
                // 说不可以给子进程发信号了
                flag = 0;
            }
        
        }
    }
    // 退出:0
    return 0;
}
