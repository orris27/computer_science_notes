#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main()
{
    // 创建1个UDP套接字
    int sockfd;
    struct sockaddr_in addr;

    if ((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)
          handle_error("socket");

    // 绑定ip
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0) 
          handle_error("bind");


    // 构造组信息
    struct ip_mreqn group;
    group.imr_multiaddr.s_addr = inet_addr("239.0.0.2");
    group.imr_address.s_addr = inet_addr("0.0.0.0");
    group.imr_ifindex = if_nametoindex("eth0");

    // 允许套接字能发送组播消息:参数=组信息
    if(setsockopt(sockfd,IPPROTO_IP,IP_MULTICAST_IF,&group,sizeof(group))<0)
        handle_error("setsockopt");

    // 构造对方的ip地址:IP=组ip,端口=3000
    struct sockaddr_in peer_addr;
    bzero(&peer_addr, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "239.0.0.2" , &peer_addr.sin_addr.s_addr);
    peer_addr.sin_port = htons(3000);
    // 定义传输的数据
    char send_buf[1024];
    strcpy(send_buf,"hello");
    // while(1)
    while(1)
    {
        
        printf("send msg\n");
        // 发送数据给对方
        sendto(sockfd,send_buf,strlen(send_buf),0,(struct sockaddr*)&peer_addr,sizeof(peer_addr));
        // 睡一会儿
        sleep(1);
    
    }
    // 关闭套接字
    close(sockfd);
    // 退出:0
    return 0;

}
