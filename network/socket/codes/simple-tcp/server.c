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
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
      ERR_EXIT("socket");
    
    //create an ip addr and bind the socket to it
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0)
      ERR_EXIT("bind");

    //listen
    if(listen(sockfd,SOMAXCONN)<0)
      ERR_EXIT("listen");
    
    //accept & get the connection socket
    struct sockaddr peer_addr;
    socklen_t peer_len = sizeof(peer_addr);
    int conn_sockfd;
    if ((conn_sockfd = accept(sockfd,(struct sockaddr*)&peer_addr,&peer_len))<0)
      ERR_EXIT("accept");


    //connect 
    char recv_buf[1024];
    while(1)
    {
        memset(recv_buf,0,sizeof(recv_buf));
        int ret = read(conn_sockfd,&recv_buf,sizeof(recv_buf));
        fputs(recv_buf,stdout);
        write(conn_sockfd,recv_buf,ret);
    }


    close(conn_sockfd);
    close(sockfd);
    return 0;



    


    
    


}
