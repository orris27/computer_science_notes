#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


// 定义MSGMAX
#define MSGMAX 8192
#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

// 定义消息的结构体,MSGMAX
struct msgbuf {
    long mtype;    
    char mtext[MSGMAX]; //man中提供的是1
};
//
/* 
 * 提供回射的客户端
 */
void handle_echo_client(int msgid)
{
    // 获取当前的pid
    int pid = getpid();
    // 定义1个消息
    struct msgbuf *p_buf = (struct msgbuf *)malloc(sizeof(long) + MSGMAX);
    // 赋值消息的消息类型=1
    p_buf->mtype = 1;
    // 赋值pid给消息的实际数据的前4个字节.
    *(int*)p_buf->mtext = pid;
    // while(读取键盘中的数据到消息实际数据第4个字节后面中)
    while(fgets(p_buf->mtext+4,sizeof(p_buf->mtext+4),stdin) != NULL)
    {
        // 发送消息类型为1的消息
        p_buf->mtype = 1;
        if((msgsnd(msgid,p_buf,MSGMAX,0)) == -1)
              ERR_EXIT("msgsnd");
        // 清洗消息实际数据第4个字节后面
        memset(p_buf->mtext+4,0,sizeof(p_buf->mtext)-4);
        // 接收消息类型为pid的消息
        if ((msgrcv(msgid,p_buf,MSGMAX,pid,0)) == -1)
              ERR_EXIT("msgrcv");
        // 打印刚接收到的消息
        fputs(p_buf->mtext+4,stdout);
        // 清洗消息实际数据第4个字节后面
        memset(p_buf->mtext+4,0,sizeof(p_buf->mtext)-4);
        
    }
}
int main()
{
    // 打开消息队列
    int msgid = msgget(1234,0);
    if(msgid == -1)
          ERR_EXIT("msgget");
    // 提供回射的客户端
    handle_echo_client(msgid);
    // return 0


}
