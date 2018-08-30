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
    if(argc != 2)
    {
        fprintf(stderr,"Usage:%s <priority>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    // 打开消息队列
    mqd_t mqid = mq_open("/abc",O_RDWR);
    if(mqid == (mqd_t) -1)
          ERR_EXIT("mq_open");

    // 构造消息
    Student orris;
    strcpy(orris.name,"orris");
    orris.age = 15;

    // 发送消息
    if((mq_send(mqid,(char*)&orris,sizeof(orris),atoi(argv[1]))) == -1)
        ERR_EXIT("mq_send");
    // 关闭消息队列
    if((mq_close(mqid)) == -1)
          ERR_EXIT("mq_close");
    // return 0
    return 0;

}
