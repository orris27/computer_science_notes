#include <fcntl.h>   
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

int main()
{
    // 打开消息队列
    mqd_t mqid = mq_open("/abc",O_RDONLY);
    if(mqid == (mqd_t) -1)
          ERR_EXIT("mq_ope");

    // 定义消息队列的属性结构变量
    struct mq_attr attr;
    // 获取消息队列的属性
    if((mq_getattr(mqid,&attr)) == -1)
        ERR_EXIT("mq_getattr");
    // 打印消息队列的属性
    printf("maxmsg=%ld\n",attr.mq_maxmsg);
    printf("msgsize=%ld\n",attr.mq_msgsize);
    printf("curmsgs=%ld\n",attr.mq_curmsgs);

    if((mq_close(mqid)) == -1)
          ERR_EXIT("mq_close");
    // return 0
    return 0;
}
