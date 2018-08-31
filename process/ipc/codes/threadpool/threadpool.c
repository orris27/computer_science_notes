#include "threadpool.h"

/*
 * 线程的入口函数
 */
void* handle_thread(void *arg)
{
    // 定义超时时间
    struct timespec abstime;
    // 获取当前时间
    clock_gettime(CLOCK_REALTIME,&abstime);
    // 增加超时时间2s
    abstime.tv_sec = 2;


    // 打印说xx线程正在启动
    printf("0x%x is starting...\n",(int)pthread_self());
    // (这个线程是消费者)
    // 获取参数(线程池变量)
    threadpool_t *pool = (threadpool_t*)arg;
    // 释放参数(不用释放,因为它本身不是malloc出来的)
    // while(1)
    while(1)
    {
        // 定义超时标记timeout_flag
        int timeout_flag = 0;
        // 锁定
        condition_lock(&pool->ready);
        // 更新空闲的线程数:+1
        pool->idle++;
        // 等待队列有任务到来或者线程池销毁通知
        // while(队列中没有任务 && 线程池没有被销毁)
        while(pool->first == NULL && pool->quit == 0)
        {
            // 打印说xx线程正在等待任务
            printf("threadid=0x%x is waiting for task...\n",(int)pthread_self());
            // 等待条件变量
            //condition_wait(&pool->ready);
            // 等待条件变量,如果过了超时时间,就不等待了
            int ret = condition_timedwait(&pool->ready,&abstime);

            // 如果是超时了
            if(ret == ETIMEDOUT)
            {
                // 打印说等待超时了
                printf("0x%x is timeout\n",(int)pthread_self());
                // 置超时时间的标志
                timeout_flag = 1;
                // break
                break;
            }
             
        }
        // 更新空闲的线程数:-1
        pool->idle--;
        // 如果等待到的事队列中有任务
        if(pool->first != NULL)
        {
            // 取出队头的任务
            task_t *p_task = pool->first;
            // (删除链表的头指针)
            // 如果当前只有1个任务(first指向第一个数据,而last指向最后一个数据)
            if(pool->first == pool->last)
            {
                // 置first=last都指向NULL
                pool->first = pool->last = NULL;
            }
            // 如果当前有多个任务
            else
            {
                // 移动头指针到下一个 
                pool->first = pool->first->next;
            }
            // 解锁(生产者进程可以添加任务,消费者进程可以执行/等待任务)
            condition_unlock(&pool->ready);
            // 执行任务里的函数:参数=任务里的参数
            p_task->run(p_task->arg);
            // 释放任务对象
            free(p_task);
            // 锁定
            condition_lock(&pool->ready);
        }
        // 如果等待的是线程池要销毁 && 任务都处理完毕
        if( pool->quit == 1 && pool->first == NULL )
        {
            // (可以销毁线程了)
            // 更新当前线程数
            pool->count--;
            // 如果线程数减为0了,就发起通知
            if(pool->count == 0)
                condition_signal(&pool->ready);
            // 解锁
            condition_unlock(&pool->ready);
            // break;
            break;
        }
        // 如果超时了 并且没有任务
        if(timeout_flag == 1 && pool->first == NULL)
        {

            // 更新当前的线程数
            pool->count--;
            // 如果线程数减为0了,就发起通知
            if(pool->count == 0)
                condition_signal(&pool->ready);
            // 解锁
            condition_unlock(&pool->ready);
            // break;
            break;
        }

        // 解锁
        condition_unlock(&pool->ready);
    }
    // 打印说线程处于销毁的状态
    printf("0x%x is destroying...\n",(int)pthread_self());
    // 返回NULL
    return NULL;
}

void threadpool_init(threadpool_t *pool,int max_threads)
{
    // 初始化条件变量
    condition_init(&pool->ready);
    // 填充线程池变量:first=NULL,last=NULL,当前线程数=0,当前空闲线程数=0,最大的线程数=threads,quit=0
    pool->first = NULL;
    pool->last = NULL;
    pool->count = 0;
    pool->idle = 0;
    pool->max_threads = max_threads;
    pool->quit = 0;
}
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg),void *arg)
{
    // 构造任务的结构变量
    task_t *p_task = (task_t*)malloc(sizeof(task_t));
    // 填充任务的结构变量:入口函数,参数,下一个任务的指针=NULL
    p_task->run = run;
    p_task->arg = arg;
    p_task->next = NULL;
    // 加锁(因为线程池资源生产者和消费者都能去访问的)
    condition_lock(&pool->ready);
    // 将任务添加到队列最后
    // 如果第一次添加
    if(pool->first == NULL)
    {
        // 就放到first
        pool->first = p_task;
    }
    // 如果不是第一次添加
    else
    {
        // 就放到last的后面
        pool->last->next = p_task;
    }
    // 更新尾指针
    pool->last = p_task;
    // 如果有线程在等待状态
    if(pool->idle > 0)
    {
        // 发送信号唤醒线程
        condition_signal(&pool->ready);
    }
    // 如果没有空闲线程&当前线程数小于最大线程数
    else if (pool->idle == 0 && pool->count < pool->max_threads)
    {
        // 创建1个线程:传递的参数=线程池变量
        pthread_t tid;
        int ret;
        if((ret = pthread_create(&tid,NULL,handle_thread,pool)) != 0 )
        {
            fprintf(stderr,"pthread_create:%s\n",strerror(ret));
            exit(EXIT_FAILURE);
        }

        // 更新当前线程池数
        pool->count++;
    }
    // 解锁
    condition_unlock(&pool->ready);
}
void threadpool_destroy(threadpool_t *pool)
{
    // 如果已经调用过销毁了
    if(pool->quit == 1)
        // 退出函数
        return;
    // 加锁(保护下变量)
    condition_lock(&pool->ready);
    // 置quit为1,表明已经销毁了
    pool->quit = 1;
    // 如果当前有线程
    if(pool->count > 0)
    {
        // 如果有等待状态的线程
        if(pool->idle > 0)
            // 发起广播(等待状态的线程会收到广播,并且发现quit=1,然后就退出了;)
            condition_broadcast(&pool->ready);
        // 如果当前仍有线程
        else
            // 等待销毁的线程给我发起通知
            condition_wait(&pool->ready);
    }
    // 解锁 
    condition_unlock(&pool->ready);
        
}
