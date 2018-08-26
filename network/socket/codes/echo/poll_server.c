#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

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
    //wait(NULL);
    //wait进程仅仅等待第一个子进程的退出
    //调用waitpid函数来处理子进程的状态
    //wait(NULL);
    //waitpid(-1,NULL,WNOHANG);
    //循环调用waitpid去轮询子进程,如果检测到他们是退出状态,我们就释放该僵尸进程
    while(waitpid(-1,NULL,WNOHANG)>0);
}

int main()
{

    //定义套接字的总数
    const int MAX_TOTAL = 2048;
    //修改文件描述符为MAX_TOTAL
    struct rlimit rl;
    rl.rlim_cur = MAX_TOTAL;
    rl.rlim_max = MAX_TOTAL;
    if (setrlimit(RLIMIT_NOFILE,&rl) <0)
            ERR_EXIT("getrlimit");


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




    //定义存放poll执行的套接字的数组
    struct pollfd client_sockets[MAX_TOTAL];
    for(int i=0;i<MAX_TOTAL;++i)
        client_sockets[i].fd = -1;
    //添加监听套接字
    client_sockets[0].fd = sockfd;
    client_sockets[0].events = POLLIN;

    //定义连接套接字数组的最大不空闲位置,初始值为0
    int max_pos = 0;
    //开始循环:重置读集合,使用全集合赋值的方式
    while(1)
    {
        //poll
        int nready = poll(client_sockets,max_pos+1,-1);
        //如果返回值为-1且错误提示为中断,继续循环
        if (nready == -1)
        {
            if(errno == EINTR)
                continue;
            //如果返回值为-1并且不是中断,结束进程
            else
                ERR_EXIT("poll");
        }
        //如果返回值为0,理论上是超时时间到的,但我们没有设置超时时间,所以其实不会发生=>继续循环
        else if (nready == 0)
            continue;
        //如果监听套接口产生了可读事件,说明accept不再阻塞=>接受对方的连接
        if (client_sockets[0].revents & POLLIN)
        {
            struct sockaddr_in peer_addr;
            socklen_t peer_len = sizeof(peer_addr); // 使用accept获得对方的套接字等信息时,对方套接字的长度一定要初始化
            int conn_sockfd;
            if ((conn_sockfd = accept(sockfd,(struct sockaddr*)&peer_addr,&peer_len))<0)
                ERR_EXIT("accept");
            //=>打印对方ip地址信息
            printf("peer=%s:%d\n",inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));


            //=>保存连接套接字到数组里,如果遍历了所有的范围还没有空闲位置,就输出错误,并且退出进程
            int i=0;
            for(i=0;i<MAX_TOTAL;++i)
            {
                if(client_sockets[i].fd == -1)
                {
                    client_sockets[i].fd = conn_sockfd;
                    client_sockets[i].events = POLLIN;
                    //=>更新最大不空闲位置
                    max_pos = (max_pos > i)?max_pos:i;
                    break;
                }
            }

            //=>减少nready,如果==0了,说明没有其他时间了=>continue
            if(--nready == 0)
                continue;
        }
    
        //遍历连接套接字从1(监听套接口忽略)到最大不空闲位置,对于产生了事件的连接套接字=>获取套接口缓冲区的数据到用户态的缓冲区
        for(int i=1;i<=max_pos;++i)
        {
            char recv_buf[1024];
            if(client_sockets[i].fd == -1)
                continue;
            if(client_sockets[i].revents & POLLIN)
            {
                
                int conn_sockfd = client_sockets[i].fd;
                int ret = read(conn_sockfd,&recv_buf,sizeof(recv_buf)); // 接收到的数据是通过connection套接字来的.直接将这个套接字当成文件(因为是文件描述符)来处理,所以read/write就行了
                //=>错误处理
                if (ret == 0)
                {
                    printf("client close\n");
                    //=>如果返回值为0,说明对方关闭了套接字=>将连接套接字数组的值置为-1+关闭连接套接字
                    client_sockets[i].fd = -1;
                    close(conn_sockfd);
                    break;
                }
                else if (ret == -1)
                {
                    ERR_EXIT("read");
                }
            
                //=>打印数据到标准输出
                fputs(recv_buf,stdout);
                //=>发送数据给对方
                write(conn_sockfd,recv_buf,sizeof(recv_buf));
                //=>减少nready,如果==0了,就结束遍历
                
                if(--nready == 0)
                    break;
            
            }
        }
        
    }




    return 0;
}
