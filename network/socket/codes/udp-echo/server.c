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

// 提供回射服务
void handle_echo(int sockfd)
{
    // 定义接收的缓冲区
    char recv_buf[1024];
    // 定义保存对方地址的变量
    struct sockaddr_in peer_addr;
    // 初始化地址的字节长度
    socklen_t peer_len = sizeof(peer_addr);
    // while(1)
    while(1)
    {
    
        // 清空缓冲区
        memset(recv_buf,0,sizeof(recv_buf));
        // 接收UDP数据(套接字,缓冲区,接收数据的长度,flags用0,指定对方的地址信息)+错误处理
        int ret = recvfrom(sockfd,recv_buf,sizeof(recv_buf),0,(struct sockaddr*)&peer_addr,&peer_len);
        // 如果是-1
        if (ret == -1)// 如果read出现错误,就直接退出子进程
        {
            if (errno == EINTR)
                continue;
            else //如果返回值<0,并且不是中断的信号=>退出进程,返回-1
                ERR_EXIT("read");
        }
        // 如果n>0
        else if(ret > 0)
        {
            // 输出接收到的数据
            fputs(recv_buf,stdout);
            // 发送数据给对方(套接字,发送的数据,长度,0,对方的ip地址,ip地址的长度
            sendto(sockfd,recv_buf,strlen(recv_buf),0,(struct sockaddr*)&peer_addr,sizeof(peer_addr));
        }
    // 结束循环
    }
    // 关闭套接字
    close(sockfd);


}


int main()
{
    // 创建一个套接字.UDP使用SOCK_DGRAM,第三个参数是0(让系统自己选择协议+错误处理
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)
        ERR_EXIT("socket");
    // 绑定ip地址
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0) // "专门协议地址=>通用协议地址"使用小括号类型转换就好了
        ERR_EXIT("bind");

    handle_echo(sockfd);
}
