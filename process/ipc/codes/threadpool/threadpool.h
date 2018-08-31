#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <pthread.h>
#include "condition.h"

typedef struct task 
{
    void *(*run)(void *arg);   //指针函数（即这个任务要去做什么事情）
    void *arg;                 // 参数
    struct task *next;         // 指向下一个任务的指针
} task_t;

typedef struct threadpool //线程池结构体
{
    condition_t ready; // 任务准备就绪或者线程池销毁通知
    task_t *first;     // 任务队列头指针
    task_t *last;      // 任务队列尾指针
    int count;         // 线程池中当前线程数
    int idle;          // 线程池中当前正在等待任务的线程数
    int max_threads;   // 线程池中最大允许的线程数
    int quit;          // 销毁线程池的时候置1
    
}threadpool_t;

void threadpool_init(threadpool_t *pool,int threads);
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg),void *arg);
void threadpool_destroy(threadpool_t *pool);

#endif
