#include <unistd.h>
#include "threadpool.h"

/*
 * 任务入口函数
 */
void *do_task(void *arg)
{
    // 获取参数
    int val = *(int*)arg;
    // 释放主线程传递过来的参数
    free(arg);
    // 打印说哪个线程正在工作哪个任务
    printf("0x%x is working on %d...\n",(int)pthread_self(),val);
    // 睡一会儿
    sleep(1);
    // 返回NULL
    return NULL;
}

int main()
{
    // 定义线程池变量
    threadpool_t pool;
    // 初始化线程池:线程数量=3
    threadpool_init(&pool,3);
    // 添加10个任务:线程池,线程入口函数,参数
    for(int i=0;i<10;++i)
    {
        int *p = (int*)malloc(sizeof(int)); // 构造传递的参数
        *p = i;
        threadpool_add_task(&pool,do_task,p);
    }

    // 睡一会儿,等待线程都执行完任务
    //sleep(15);
    // 销毁线程池变量
    threadpool_destroy(&pool);
    // return 0
    return 0;
}
