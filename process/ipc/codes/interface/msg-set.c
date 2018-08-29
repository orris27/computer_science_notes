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


    sscanf("600","%o",(unsigned int*)&msg_stat.msg_perm.mode);
    if((msgctl(msgid,IPC_SET,&msg_stat) == -1))
        ERR_EXIT("msgctl");

    return 0;


}
