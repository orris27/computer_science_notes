#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
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


int main()
{
    struct rlimit rl;
    rl.rlim_cur = 2048;
    rl.rlim_max = 2048;
    if (setrlimit(RLIMIT_NOFILE,&rl) <0)
            ERR_EXIT("getrlimit");


    int count = 0;
    while(1)
    {
    
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

        //输出结果到屏幕上
        printf("count=%d\n",++count);
    }

    return 0;
}
