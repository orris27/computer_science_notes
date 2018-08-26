#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>


#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

void handle_sigpipe(int sig)
{
    printf("in handler\n");
}


int main()
{

    signal(SIGPIPE,handle_sigpipe);


    // create a clinet socket
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
      ERR_EXIT("socket");
    
    //create an ip addr & connect the socket to it
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188); 
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0)
      ERR_EXIT("connect");

    char send_buf[1024];
    char recv_buf[1024];

    while(fgets(send_buf,sizeof(send_buf),stdin))
    {
        // 发送数据只要往套接字里写入数据就好了
        write(sockfd,send_buf,1);
        printf("1 & 2\n");
        write(sockfd,send_buf+1,sizeof(send_buf)-1);
        printf("after 2\n");
        // 接收数据只要从套接字里读取数据就好了
        int ret = read(sockfd,recv_buf,sizeof(recv_buf));
        if(ret == 0)
        {
            printf("server close\n");
            break;
        }
        else if(ret < 0)
            ERR_EXIT("read");

        fputs(recv_buf,stdout);
        memset(send_buf,0,sizeof(send_buf));
        memset(recv_buf,0,sizeof(recv_buf));
    }

    // close all the sockets
    close(sockfd);


    return 0;



    


    
    


}
