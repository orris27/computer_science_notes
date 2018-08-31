#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>   
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

// 定义消费者的线程个数
#define CONSUMER_THREADS 2
// 定义生产者的线程个数
#define PRODUCER_THREADS 1
// 定义当前生产的产品id
int produce_id = 0;
// 定义当前消费的产品id
int consume_id = 0;

// 定义互斥锁
pthread_mutex_t mutex;

// 定义保存线程id的数组:个数=消费者的线程个数+生产者的线程个数
pthread_t threads[CONSUMER_THREADS+PRODUCER_THREADS];

// 定义条件变量
pthread_cond_t cond;
// 定义仓库里生产出来的产品数量:初始值=0
int total = 0;

/*
 * 消费者线程的入口函数
 */
void* handle_consume(void *arg)
{   
    // 获取线程id
    int subscript = *(int*)arg;
    free(arg);
    // while(1)
    while(1)
    {

        // (假定生产的产品(缓冲区)的大小=块大小)
        // 锁定"占用位置"的资源
        pthread_mutex_lock(&mutex);
        // while(仓库里没有产品)
        while(total == 0)
        {
            // 打印说当前处于等待状态
            printf("CONSUMER:%d is waiting...\n",subscript);
            // 等待仓库里有产品的条件
            pthread_cond_wait(&cond,&mutex);
        }
        // 打印说xx线程等待到了条件
        printf("CONSUMER:%d ends waiting\n",subscript);
        // 打印说xx线程正在开始消费产品
        printf("CONSUMER:%d is consuming...\n",subscript);
        // 消费产品:直接自减仓库产品数量
        total--;
        // 解锁"占用位置"的资源
        pthread_mutex_unlock(&mutex);
        // 睡3s
        sleep(3);
    }


    // 返回NULL
    return NULL;
}

/*
 * 生产者线程的入口函数
 */
void* handle_produce(void *arg)
{   
    // 获取当前线程在线程数组中的下标
    int subscript = *(int*)arg;
    free(arg);
    // while(1)
    while(1)
    {
        // 锁定"占用位置"的资源
        pthread_mutex_lock(&mutex);
        // 打印说xx线程正在开始生产产品
        printf("PRODUCER:%d is producing...\n",subscript);
        // 生产产品:仓库内的产品数量+1
        total++;
        // 打印说xx线程发起了条件变量的通知
        printf("PRODUCER:%d signals\n",subscript);
        // 通知条件变量
        pthread_cond_signal(&cond);
        // 解锁"占用位置"的资源
        pthread_mutex_unlock(&mutex);
        // 睡1s
        sleep(1);
    }

    // 返回NULL
    return NULL;
}



int main()
{
    // 初始化互斥锁:属性=NULL
    pthread_mutex_init(&mutex,NULL);
    // 初始化条件变量g_cond
    // 创建消费者的线程,保存到线程的数组前面:传参=数组下标
    int i = 0;
    for(i=0;i<CONSUMER_THREADS;++i)
    {
        int ret;
        int *p = (int*)malloc(sizeof(int)); // 构造传递的参数
        *p = i;

        if((ret = pthread_create(&threads[i],NULL,handle_consume,p)) != 0 )
        {
            fprintf(stderr,"pthread_create:%s\n",strerror(ret));
            exit(EXIT_FAILURE);
        }
    }

    sleep(1);
    // 创建生产者的线程,保存到线程的数组后面:传参=数组下标
    int ret;
    for(i=0;i<PRODUCER_THREADS;++i)
    {
        int *p = (int*)malloc(sizeof(int)); // 构造传递的参数
        *p = i;

        if((ret = pthread_create(&threads[CONSUMER_THREADS+i],NULL,handle_produce,p)) != 0 )
        {
            fprintf(stderr,"pthread_create:%s\n",strerror(ret));
            exit(EXIT_FAILURE);
        }
    }
    
    // 等待线程结束
    for(i=0;i<CONSUMER_THREADS+PRODUCER_THREADS;++i)
    {
        if((ret = pthread_join(threads[i],NULL)) != 0 )
        {
            fprintf(stderr,"pthread_join:%s\n",strerror(ret));
            exit(EXIT_FAILURE);
        }
    }

    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);
    // 销毁条件变量

    
    // return 0
    return 0;
}
