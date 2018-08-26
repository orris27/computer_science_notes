#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

int main()
{
    // 创建多组套接字,同时去连接服务器
    const int number = 20;
    int sockfd[number];
    for (int i=0;i<number;++i)
    {
        // create a clinet socket
        if ((sockfd[i] = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
          ERR_EXIT("socket");
        
        //create an ip addr & connect the socket to it
        struct sockaddr_in addr;
        memset(&addr,0,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5188); 
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if(connect(sockfd[i],(struct sockaddr*)&addr,sizeof(addr))<0)
          ERR_EXIT("connect");

    }
    char send_buf[1024];
    char recv_buf[1024];

    while(fgets(send_buf,sizeof(send_buf),stdin))
    {
        // 发送数据只要往套接字里写入数据就好了
        write(sockfd[0],send_buf,sizeof(send_buf));
        // 接收数据只要从套接字里读取数据就好了
        read(sockfd[0],recv_buf,sizeof(recv_buf));
        fputs(recv_buf,stdout);
        memset(send_buf,0,sizeof(send_buf));
        memset(recv_buf,0,sizeof(recv_buf));
    }

    // close all the sockets
    for (int i=0;i<number;++i)
        close(sockfd[i]);

     
   
    return 0;



    


    
    


}
