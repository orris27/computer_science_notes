#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
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



void handle_client(int sockfd)
{

    char send_buf[1024];
    char recv_buf[1024];


    //定义读集合
    fd_set rset;
    //初始化读集合
    FD_ZERO(&rset);

    //检测到的事件个数
    int nready;
    //添加文件描述符
    //获得标准输入的文件描述符:fileno(stdin)
    int stdinfd = fileno(stdin);
    //判断最大的文件描述符
    int maxfd = (stdinfd>sockfd)?stdinfd:sockfd;
    //开始循环:添加文件描述符到读集合中
    while(1)
    {
        FD_SET(stdinfd,&rset);
        FD_SET(sockfd,&rset);
        //select操作
        nready = select(maxfd+1,&rset,NULL,NULL,NULL);
        //错误处理:返回值为-1就说明失败了.一般为设置了超时时间导致的
        if(nready == -1)
            ERR_EXIT("select");
        //这里我们返回值为0也继续执行
        else if (nready == 0)
            continue;
        //套接口/标准输入产生了可读事件
        //如果套接口在读集合中,就处理套接口内容
        if(FD_ISSET(sockfd,&rset))
        {
            //读取套接口缓冲区里的数据到用户态的缓冲区中
            int ret = read(sockfd,recv_buf,sizeof(recv_buf));
            //错误处理
            if(ret == 0)                 
            {                            
                printf("server close\n");
                break;                   
            }                            
            else if(ret < 0)             
                  ERR_EXIT("read");        

            //输出用户态缓冲区数据到标准输出
            fputs(recv_buf,stdout);
            //清空用户态缓冲区
            memset(recv_buf,0,sizeof(recv_buf));
        }
        //如果标准输入在读集合中,就处理键盘输入
        if(FD_ISSET(stdinfd,&rset))
        {
            //获得键盘输入
            if(fgets(send_buf,sizeof(send_buf),stdin) == NULL)
                break;
            //发送
            write(sockfd,send_buf,sizeof(send_buf));
            //清空
            memset(send_buf,0,sizeof(send_buf));
        }
        
    
    }
    //关闭套接字
    close(sockfd);
}
int main()
{

    signal(SIGPIPE,SIG_IGN);


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

    handle_client(sockfd);

    return 0;
}
