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
#define PRODUCER_THREADS 4
// 定义缓冲区数组的元素个数
#define BUFFER_ELEMENTS 10
// 定义缓冲区数组
int buf[BUFFER_ELEMENTS];
// 定义消费的起点索引(环形)
int rd_index = 0;
// 定义生产的起点索引(环形)
int wr_index = 0;
// 定义当前生产的产品id
int produce_id = 0;
// 定义当前消费的产品id
int consume_id = 0;

// 定义生产者的信号量
sem_t sem_produce;
// 定义消费者的信号量
sem_t sem_consume;
// 定义互斥锁
pthread_mutex_t mutex;

// 定义保存线程id的数组:个数=消费者的线程个数+生产者的线程个数
pthread_t threads[CONSUMER_THREADS+PRODUCER_THREADS];


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

        // 打印说xx线程正在等待消费产品
        printf("CONSUMER:%d is waiting to consume...\n",subscript);
        // (假定生产的产品(缓冲区)的大小=块大小)
        // 申请"仓库还能消费"的资源
        if((sem_wait(&sem_consume)) == -1)
            handle_error("sem_wait");
        // 锁定"占用位置"的资源
        pthread_mutex_lock(&mutex);
        // 打印输出仓库的当前状态:"下标:null/产品id/当前要在这里消费产品"
        for(int i=0;i<BUFFER_ELEMENTS;++i)
        {
            if(buf[i] == -1)
            {
                if(i == rd_index)
                    printf("BUFFER:%02d empty ***\n",i);
                else 
                    printf("BUFFER:%02d empty\n",i);
            }
            else
            {
               printf("BUFFER:%02d %d\n",i,buf[i]); 
            }
        }
        // 打印说xx线程正在开始生产产品"产品id"
        printf("%d is consuming %d\n",subscript,produce_id);
        // 消费产品:更新产品数组里的元素=产品id=>-1
        // 1. 计算读取位置
        // 2. 更新元素为-1
        buf[rd_index] = -1;
        // 3. 更新消费的位置:环形更新取余
        rd_index = (rd_index+1) % BUFFER_ELEMENTS;
        // 解锁"占用位置"的资源
        pthread_mutex_unlock(&mutex);
        // 归还"仓库还能生产"的资源
        if((sem_post(&sem_produce)) == -1)
            handle_error("sem_post");
        // 睡1s
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
        // 打印说等待生产产品
        printf("PRODUCER:%d is waiting to produce...\n",subscript);
        // 申请"仓库还能生产"的资源
        if((sem_wait(&sem_produce)) == -1)
            handle_error("sem_wait");
        // 锁定"占用位置"的资源
        pthread_mutex_lock(&mutex);
        // 打印输出仓库的当前状态:"下标:null/产品id/当前要在这里生产产品"
        for(int i=0;i<BUFFER_ELEMENTS;++i)
        {
            if(buf[i] == -1)
            {
                if(i == wr_index)
                    printf("BUFFER:%02d empty ***\n",i);
                else 
                    printf("BUFFER:%02d empty\n",i);
            }
            else
            {
               printf("BUFFER:%02d %d\n",i,buf[i]); 
            }
        }

        // 打印说xx线程正在开始生产产品"产品id"
        printf("%d is producing %d\n",subscript,produce_id);
        // 生产产品:更新产品数组里的元素为产品id
        // 1. 计算写入位置:wr_index
        // 2. 更新元素为产品id
        buf[wr_index] = produce_id++;
        // 3. 更新生产的位置:环形更新取余
        wr_index = (wr_index+1) % BUFFER_ELEMENTS;
        // 解锁"占用位置"的资源
        pthread_mutex_unlock(&mutex);
        // 归还"仓库还能消费"的资源
        if((sem_post(&sem_consume)) == -1)
            handle_error("sem_post");
        // 睡1s
        sleep(1);
    }

    // 返回NULL
    return NULL;
}



int main()
{
    // 初始化仓库里的所有产品:产品id=-1
    memset(buf,-1,sizeof(buf));
    // 初始化无名的生产者的信号量:初始值=缓冲区数组的元素个数
    if((sem_init(&sem_produce,0,BUFFER_ELEMENTS)) == -1)
        handle_error("sem_init");
    // 初始化无名的消费者的信号量:初始值=0
    if((sem_init(&sem_consume,0,0)) == -1)
        handle_error("sem_init");

    // 初始化互斥锁:属性=NULL
    pthread_mutex_init(&mutex,NULL);
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

    // 销毁2个信号量
    if((sem_destroy(&sem_produce)) == -1)
        handle_error("sem_destroy");
    if((sem_destroy(&sem_consume)) == -1)
        handle_error("sem_destroy");
    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);

    
    // return 0
    return 0;
}
