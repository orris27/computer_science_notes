#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/epoll.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)


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
            handle_error("getrlimit");


    //signal(SIGCHLD,SIG_IGN);
    signal(SIGCHLD,handle_sigchld);

    //创建服务器的套接字.套接字是一个文件描述符
    //使用什么协议(这里是TCP/IP协议)
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
      handle_error("socket");
    
    //create an ip addr & bind the socket to it
    struct sockaddr_in addr; // 使用IPV4的协议结构体,但是绑定给套接字的时候,地址必须是通用协议地址,所以要类型转换
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188); // 绑定端口号的时候使用网络字节序
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 老师推荐使用,说这个会绑定当前IP
    //addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 也可以使用这个.点分IP=>网络字节序的32位无符号证书
  
    
    
    int reuse_on=1;
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse_on,sizeof(reuse_on))<0)
      handle_error("setsockopt");

    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0) // "专门协议地址=>通用协议地址"使用小括号类型转换就好了
      handle_error("bind");

    //listen
    if(listen(sockfd,SOMAXCONN)<0) // SOMAXCONN是服务器套接字允许建立的最大队列,包括未连接+已连接的队列
      handle_error("listen");

    // 定义epoll红黑树(连接的套接字)的最大节点
    const int size = 10;
    // 定义接收连接套接字数据的缓冲区recv_buf
    char recv_buf[1024];
    // 定义epoll的红黑树
    int epfd = epoll_create(size);
    if(epfd == -1)
        handle_error("epoll_create");
    // 构造epoll红黑树节点struct epoll_event类型
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = sockfd;
    // 添加监听套接字到红黑树
    if((epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&event)) == -1)
        handle_error("epoll_ctl");
    // while(1)
    while(1)
    {
        // 定义保存可读集合的数组
        struct epoll_event events[size];
        // 等待epoll内的文件描述符的可读操作,并获得可操作的集合的个数nready+错误处理
        int nready = epoll_wait(epfd,events,size,-1);
        if (nready == -1)
        {
            if (errno == EINTR)
                continue;
            else 
                handle_error("epoll_wait");
        }
    
        // 遍历集合.for(i:0=>nready)
        for(int i=0;i<nready;++i)
        {
            // 过滤掉不是可读的事件
            if(!(events[i].events & EPOLLIN))
                continue;
            
            // 如果是监听套接字
            if(events[i].data.fd == sockfd)
            {
                // 接收连接,并获得连接套接字
                
                struct sockaddr_in peer_addr;
                socklen_t peer_len = sizeof(peer_addr); // 使用accept获得对方的套接字等信息时,对方套接字的长度一定要初始化
                int conn_sockfd;
                if ((conn_sockfd = accept(sockfd,(struct sockaddr*)&peer_addr,&peer_len))<0)
                    handle_error("accept");
                //=>打印对方ip地址信息
                printf("peer=%s:%d\n",inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));
                    
                // 添加连接套接字到epoll红黑树中
                event.events = EPOLLIN;
                event.data.fd = conn_sockfd;
                // 添加监听套接字到红黑树
                if((epoll_ctl(epfd,EPOLL_CTL_ADD,conn_sockfd,&event)) == -1)
                    handle_error("epoll_ctl");
                 
            }
            // 如果不是监听套接字
            else
            {
                // 清洗缓冲区
                memset(recv_buf,0,sizeof(recv_buf));
                // 读取连接套接字数据到缓冲区中
                    
                int conn_sockfd = events[i].data.fd;
                int ret = read(conn_sockfd,&recv_buf,sizeof(recv_buf)); // 接收到的数据是通过connection套接字来的.直接将这个套接字当成文件(因为是文件描述符)来处理,所以read/write就行了
                //=>错误处理
                if (ret == 0)
                {
                    printf("client close\n");
                    
                    // 删除epoll红黑树里的连接套接字
                    if((epoll_ctl(epfd,EPOLL_CTL_DEL,conn_sockfd,NULL)) == -1)
                        handle_error("epoll_ctl");

                    // 关闭套接字
                    close(conn_sockfd);
                    break;
                }
                else if (ret == -1)
                {
                    handle_error("read");
                }
            
                //=>打印数据到标准输出
                fputs(recv_buf,stdout);
                //=>发送数据给对方
                write(conn_sockfd,recv_buf,sizeof(recv_buf));

            
            }
        }
    }

    return 0;
}
