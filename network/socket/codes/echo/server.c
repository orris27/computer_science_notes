#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)



void handle_client(int conn_sockfd)
{
    //connect 
    char recv_buf[1024]; // 接收到的数据要输出到屏幕上,所以需要设置字符数组作为缓冲区,而这个也是1024字节
    while(1)
    {
        memset(recv_buf,0,sizeof(recv_buf));
        int ret = read(conn_sockfd,&recv_buf,sizeof(recv_buf)); // 接收到的数据是通过connection套接字来的.直接将这个套接字当成文件(因为是文件描述符)来处理,所以read/write就行了
        if (ret == 0) // 对方套接字如果关闭的话,就会返回0(表示发送过来的数据大小为0字节). 如果对方套接字关闭了,我们也结束死循环
        {
            printf("client close\n");
            break;
        }
        else if (ret == -1)// 如果read出现错误,就直接退出子进程
        {
            ERR_EXIT("read");
        }
        fputs(recv_buf,stdout);
        write(conn_sockfd,recv_buf,ret);
    }
}


void handle_sigchld(int sig)
{
    //调用wait函数子进程的状态
    wait(NULL);
    //wait进程仅仅等待第一个子进程的退出
}

int main()
{


    //signal(SIGCHLD,SIG_IGN);
    signal(SIGCHLD,handle_sigchld);

    //创建服务器的套接字.套接字是一个文件描述符
    //使用什么协议(这里是TCP/IP协议)
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
      ERR_EXIT("socket");
    
    //create an ip addr & bind the socket to it
    struct sockaddr_in addr; // 使用IPV4的协议结构体,但是绑定给套接字的时候,地址必须是通用协议地址,所以要类型转换
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188); // 绑定端口号的时候使用网络字节序
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 老师推荐使用,说这个会绑定当前IP
    //addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 也可以使用这个.点分IP=>网络字节序的32位无符号证书
  
    
    
    int reuse_on=1;
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse_on,sizeof(reuse_on))<0)
      ERR_EXIT("setsockopt");

    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0) // "专门协议地址=>通用协议地址"使用小括号类型转换就好了
      ERR_EXIT("bind");

    //listen
    if(listen(sockfd,SOMAXCONN)<0) // SOMAXCONN是服务器套接字允许建立的最大队列,包括未连接+已连接的队列
      ERR_EXIT("listen");
    
    //accept & get the connection socket
    struct sockaddr_in peer_addr;
    socklen_t peer_len = sizeof(peer_addr); // 使用accept获得对方的套接字等信息时,对方套接字的长度一定要初始化
    int conn_sockfd;


    while(1)
    {
        if ((conn_sockfd = accept(sockfd,(struct sockaddr*)&peer_addr,&peer_len))<0)
          ERR_EXIT("accept");

        // 打印对方的ip地址和端口号
        printf("peer=%s:%d\n",inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));

        pid_t pid;
        pid = fork();

        if (pid == 0)//子进程应该处理这个连接
        {
            close(sockfd);// 不同进程间资源独立.所以子进程里也有监听套接字,而子进程不需要监听套接字,所以关闭
            handle_client(conn_sockfd);
            exit(EXIT_SUCCESS);
        }
        else
        {
            close(conn_sockfd); // 同理,主进程不需要连接套接字
        }

    }

    // close all the sockets
    close(conn_sockfd);
    close(sockfd);
    return 0;
}
