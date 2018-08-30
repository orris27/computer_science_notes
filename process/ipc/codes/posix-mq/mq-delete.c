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

    // 删除消息队列
    
    if((mq_unlink("/abc")) == -1)
        ERR_EXIT("mq_unlink");

    // 解除连接
//    if((mq_close(mqid)) == -1)
 //         ERR_EXIT("mq_close");
    // return 0
    return 0;
}
