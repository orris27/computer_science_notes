#include <fcntl.h>   
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

int main()
{
    // 创建消息队列:名字,读写|创建,权限,NULL+错误处理

    mqd_t mqid = mq_open("/abc",O_RDWR|O_CREAT,0666,NULL);
    if(mqid == (mqd_t) -1)
        ERR_EXIT("mq_ope");
    // 关闭消息队列
    if((mq_close(mqid)) == -1)
        ERR_EXIT("mq_close");
    // return 0
    return 0;

}
