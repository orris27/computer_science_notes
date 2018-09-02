#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
    // 创建1个UDP套接字
    int sockfd;
    struct sockaddr_in addr;

    // 绑定ip:端口=3000
    if ((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)
          handle_error("socket");
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3000);
    inet_pton(AF_INET, "0.0.0.0" , &addr.sin_addr.s_addr);
    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0) 
          handle_error("bind");
    // 定义对方的ip地址
    //struct sockaddr_in peer_addr;
    //socklen_t peer_len = sizeof(peer_addr);
    // 定义接收数据的缓冲区
    char recv_buf[1024];
    // while(读取套接字里面的数据)
    while(1)
    {
        printf("waiting...\n");
        int ret = recvfrom(sockfd,recv_buf,sizeof(recv_buf),0,NULL,0);
        printf("waiting finished\n");
        if (ret == -1)// 如果read出现错误,就直接退出子进程
        {
            if (errno == EINTR)
                continue;   
            else //如果返回值<0,并且不是中断的信号=>退出进程,返回-1
                handle_error("read");
        }
        else if(ret > 0)
        { // 如果返回正确=>我们怎么处理
            
            // 打印数据
            fputs(recv_buf,stdout);
            fflush(stdout);
            // 清洗数据
            memset(recv_buf,0,sizeof(recv_buf));
        }

    }
    // 关闭套接字
    if((close(sockfd)) == -1)
        handle_error("close");
    // 退出:0
    return 0;

}
