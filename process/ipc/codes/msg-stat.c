#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)
int main()
{
    //打开消息队列
    int msgid = msgget(1234,0);
    if(msgid == -1)
          ERR_EXIT("msgget");

    //定义消息队列状态的变量
    struct msqid_ds msg_stat;
    //获取消息队列的状态
    if((msgctl(msgid,IPC_STAT,&msg_stat) == -1))
        ERR_EXIT("msgctl");
    //输出消息队列的权限
    printf("mode=%o\n",msg_stat.msg_perm.mode);
    //输出消息队列中消息的个数
    printf("number=%d\n",(int)msg_stat.msg_qnum);
    //输出消息队列当前的字节数
    printf("bytes=%ld\n",msg_stat.__msg_cbytes);
    //输出消息队列能容纳的最大字节数
    printf("msgmnb=%d\n",(int)msg_stat.msg_qbytes);
    //return 0
    return 0;


}
