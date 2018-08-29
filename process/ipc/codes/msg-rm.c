#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)


int main()
{
    // 打开消息队列+错误处理
    int msgid = msgget(1234,0);
    if(msgid == -1)
        ERR_EXIT("msgget");
    // 删除消息队列
    if((msgctl(msgid,IPC_RMID,NULL)) == -1)
        ERR_EXIT("msgctl");
    // return 0
    return 0;

}
