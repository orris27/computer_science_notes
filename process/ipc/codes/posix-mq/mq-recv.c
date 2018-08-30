#include <fcntl.h>   
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

typedef struct{
    char name[32];
    int age;
} Student;

int main(int argc, char* argv[])
{
    // 打开消息队列
    mqd_t mqid = mq_open("/abc",O_RDWR);
    if(mqid == (mqd_t) -1)
          ERR_EXIT("mq_open");
    // 获取单个消息的最大字节长度
    struct mq_attr attr;
    if((mq_getattr(mqid,&attr)) == -1)
          ERR_EXIT("mq_getattr");

    // 构造消息
    Student stu;
    // 定义保存优先级的变量
    unsigned int prio;
    // 接收消息
    if((mq_receive(mqid,(char*)&stu,attr.mq_msgsize,&prio)) == -1)
        ERR_EXIT("mq_send");
    // 打印接收到消息
    printf("prio=%u name=%s age=%d\n",prio,stu.name,stu.age);
    // 关闭消息队列
    if((mq_close(mqid)) == -1)
          ERR_EXIT("mq_close");
    // return 0
    return 0;

}
