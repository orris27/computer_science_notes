#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)


// 定义消息的结构体(可以使用man的参考类型)
struct msgbuf {
    long mtype;    
    char mtext[1]; 
};

/* argv[0] <bytes> <type> */
int main(int argc, char *argv[])
{
    // 错误提示命令行使用方法
    if(argc != 3)
    {
        fprintf(stderr,"Usage: %s <bytes> <type>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // 打开消息队列
    int msgid = msgget(1234,0);
    if(msgid == -1)
          ERR_EXIT("msgget");
    // 定义保存字节数的变量
    int bytes = atoi(argv[1]);
    // 定义保存消息类型的变量
    int type  = atoi(argv[2]);
    // 构造1个消息
    struct msgbuf *p_buf = (struct msgbuf *)malloc(sizeof(long) + bytes);
    // 填写这个消息的消息类型
    p_buf->mtype = type;

    // 发送消息给消息队列
    if((msgsnd(msgid,p_buf,bytes,IPC_NOWAIT)) == -1)
        ERR_EXIT("msgsnd");
    // return 0
    return 0;
}
