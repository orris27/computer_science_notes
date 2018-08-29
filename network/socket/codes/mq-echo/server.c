#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MSGMAX 8192
#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)
// 定义消息的结构体,这里用MSGMAX作为data
struct msgbuf {
    long mtype;    
    char mtext[MSGMAX]; 
};

/* 
 * 提供回射客户端的功能
 */
void handle_echo_server(int msgid)
{
    // 定义1个消息
    struct msgbuf *p_buf = (struct msgbuf *)malloc(sizeof(long) + MSGMAX);
    // 清洗消息
    memset(p_buf,0,sizeof(*p_buf));
    // while(1)
    while(1)
    {
        // 接收消息类型为1的消息
        int ret;
        ret = msgrcv(msgid,p_buf,MSGMAX,1,0);
        if(ret == -1)
              ERR_EXIT("msgrcv");
        // 获取客户端的pid.前4个字节的整型(进程id).先转换成int*再取出来
        int pid = *((int*)(p_buf->mtext));
        // 打印第4个字节后面开始的消息
        fputs(p_buf->mtext+4,stdout);
        // 发送消息类型为pid的消息
        p_buf->mtype = pid;
        if((msgsnd(msgid,p_buf,MSGMAX,0)) == -1)
              ERR_EXIT("msgsnd");
    }
}

int main()
{
    // 创建1个消息队列
    int msgid = msgget(1234,0666|IPC_CREAT);
    if(msgid == -1)
          ERR_EXIT("msgget");
    // 提供回射的服务
    handle_echo_server(msgid);
    // return 0
}
