#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>




// 定义key的变量
pthread_key_t key;


typedef struct {
    char name[32];
    int age;
}Student;
/*
 * 定义实际数据的销毁函数
 */
void destructor(void *value)
{
    // 打印说正在销毁
    printf("destroy...\n");
    // 释放掉value值(如果value值是malloc出来的话)
    free(value);
}

pthread_once_t once_control = PTHREAD_ONCE_INIT;
void handle_once()
{
    // 创建1个key
    pthread_key_create(&key,destructor);
    // 打印说正在创建key
    printf("Creating key....\n");
}

/*
 * 线程的入口地址(处理函数)
 */
void* handle_thread(void *arg)
{
    // 创建key.(线程内创建key的话,必须保证只有1个线程去创建,其他的不能去创建)
    pthread_once(&once_control,handle_once);
    // 构造value值
    Student *orris = (Student*)malloc(sizeof(Student));
    strcpy(orris->name,"orris");
    orris->age = pthread_self()%11;
    // 设置TSD的key的值
    pthread_setspecific(key,orris);
    
    // 打印说xx线程正在设定key值
    printf("threadid=0x%x is setting key\n",(int)pthread_self());
    // 获取TSD的value值
    memcpy(orris,pthread_getspecific(key),sizeof(Student));
    // 打印输出该value值
    printf("threadid=0x%x name=%s age=%d\n",(int)pthread_self(),orris->name,orris->age);
    // 睡2秒钟
    sleep(2);
    // 再次获取TSD的value值
    memcpy(orris,pthread_getspecific(key),sizeof(Student));
    // 打印输出该value值
    printf("threadid=0x%x name=%s age=%d\n",(int)pthread_self(),orris->name,orris->age);
    // 返回NULL
    return NULL;
}


int main()
{
    // 创建1个key
    //pthread_key_create(&key,destructor);
    // 创建2个线程
    int ret;
    pthread_t tid1;
    pthread_t tid2;

    if((ret = pthread_create(&tid1,NULL,handle_thread,NULL)) != 0 )
    {
        fprintf(stderr,"pthread_create:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    if((ret = pthread_create(&tid2,NULL,handle_thread,NULL)) != 0 )
    {
        fprintf(stderr,"pthread_create:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    // 等待2个线程都退出
    if((ret = pthread_join(tid1,NULL)) != 0 )
    {
        fprintf(stderr,"pthread_join:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }

    if((ret = pthread_join(tid2,NULL)) != 0 )
    {
        fprintf(stderr,"pthread_join:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    
    // 删除key
    pthread_key_delete(key);
    // return 0
    return 0;
}
