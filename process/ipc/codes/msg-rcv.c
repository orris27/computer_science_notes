#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

#define MSGMAX  8192

// 定义消息的结构体(可以使用man的参考类型)
struct msgbuf {
    long mtype;    
    char mtext[1]; 
};

/* argv[0] <bytes> <type> */
int main(int argc, char *argv[])
{
    // 定义接收的消息的消息类型
    int mtype = 0;
    // 定义接收消息函数的使用选项
    int msgflg = 0;
    // 读取命令行
    int opt;
    while(1)
    {
        opt = getopt(argc,argv,"nt:"); //"n"表示-n且没有值,"t:"表示-t且有值
        if (opt == '?') // 出现新的参数.这里不使用ERR_EXIT是因为出现了新的参数也表示成功
            exit(EXIT_SUCCESS);
        if (opt == -1) // 如果没有命令行参数
            break;
        switch(opt)
        {
            // 如果是'n'
            case 'n': 
                // 读取命令行中的msgflag,比如说是否在队列没有可读信息时直接报错
                msgflg |= IPC_NOWAIT;
                break;
            case 't': // 如果是't'
                // 读取命令行中的消息类型
                mtype = atoi(optarg);
                break;
            }
    }
    // 打开消息队列
    int msgid = msgget(1234,0);
    if(msgid == -1)
          ERR_EXIT("msgget");
    // 构造1个消息
    struct msgbuf *p_buf = (struct msgbuf *)malloc(sizeof(long) + MSGMAX);
    // 接收来自消息队列的消息+错误处理
    int ret;
    ret = msgrcv(msgid,p_buf,MSGMAX,mtype,msgflg);
    if(ret == -1)
        ERR_EXIT("msgrcv");
    // 打印接收到多少字节以及消息类型
    printf("recv bytes=%d  &  msgtype=%ld\n",ret,p_buf->mtype);
    // return 0
    return 0;
}
