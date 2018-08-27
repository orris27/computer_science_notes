#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

void echo_client(int sockfd)
{
    // 定义发送用户缓冲区
    char send_buf[1024];
    // 定义接收用户缓冲区
    char recv_buf[1024];
    memset(send_buf,0,sizeof(send_buf));
    memset(recv_buf,0,sizeof(recv_buf));
    // 定义对方的地址
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 连接套接字到该地址
    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0)
        ERR_EXIT("connect");
    // 初始化地址的字节长度
    socklen_t peer_len = sizeof(addr);
    // whlie(读取键盘上的数据到用户发送缓冲区中!=NULL)
    while(fgets(send_buf,sizeof(send_buf),stdin))
    {
        // 发送数据
        //sendto(sockfd,send_buf,strlen(send_buf),0,(struct sockaddr*)&addr,sizeof(addr));
        sendto(sockfd,send_buf,strlen(send_buf),0,NULL,0);
        // 接收数据+错误处理
        int ret = recvfrom(sockfd,recv_buf,sizeof(recv_buf),0,(struct sockaddr*)&addr,&peer_len);
        if (ret == -1)// 如果read出现错误,就直接退出子进程
        {
            if (errno == EINTR)
                continue;
            else //如果返回值<0,并且不是中断的信号=>退出进程,返回-1
                ERR_EXIT("read");
        }
        else if(ret > 0)
        {
            // 打印接收到的数据
            fputs(recv_buf,stdout); 
            // 清空2个缓冲区
            memset(send_buf,0,sizeof(send_buf));
            memset(recv_buf,0,sizeof(recv_buf));
        }
    }
    // 关闭套接字
    close(sockfd);
}

int main()
{
    // 创建套接字
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)
        ERR_EXIT("socket");
    // 提供回射客户端的功能
    echo_client(sockfd);

}
