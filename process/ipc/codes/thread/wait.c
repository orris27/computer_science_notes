#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*
 * 线程的入口地址(处理函数)
 */
void* handle_thread(void *arg)
{
    // for(i:0=>20)
    for(int i=0;i<20;++i)
    {
        // 打印A
        printf("B");
        fflush(stdout);
        // 睡20微秒(us)
        usleep(20);
    }
    // 发回"ok"
    return "ok";
}


int main()
{
    // 定义保存线程id的变量
    pthread_t tid;
    // 创建1个线程:变量,默认的属性,入口地址,参数+错误处理(从返回值获取错误代码,错误退出)
    int ret;
    if((ret = pthread_create(&tid,NULL,handle_thread,NULL)) != 0 )
    {
        fprintf(stderr,"pthread_create:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    // for(i:0=>20)
    for(int i=0;i<2;++i)
    {
        // 打印B
        printf("A");
        fflush(stdout);
        // 睡20微秒(us)
        usleep(20);
    }
    // 等待新线程
    if((ret = pthread_join(tid,NULL)) != 0 )
    {
        fprintf(stderr,"pthread_join:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }

    // return 0
    return 0;
}
