#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

int main()
{
    //创建消息队列+错误处理
    int msgid = msgget(1234,0666|IPC_CREAT);
    //int msgid = msgget(IPC_PRIVATE,0666);
    if(msgid == -1)
        ERR_EXIT("msgget");
    return 0;

}
