#include <sys/types.h> 
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

/* 
 * 提供回射服务
 */
void handle_echo_server(int conn_sockfd)
{
    // 定义接收数据的缓冲区
    char recv_buf[1024];
    // while(1)
    while(1)
    {
        // 清洗缓冲区
        memset(recv_buf,0,sizeof(recv_buf));
        // 接收数据+错误处理
        int ret = read(conn_sockfd,&recv_buf,sizeof(recv_buf));
        // 如果对方关闭了套接字
        if (ret == 0) // 对方套接字如果关闭的话,就会返回0(表示发送过来的数据大小为0字节). 如果对方套接字关闭了,我们也结束死循环
        {
            // 打印说客户端的套接字已经关闭了
            printf("client close\n");
            // break
            break;
        }
        else if (ret == -1)// 如果read出现错误,就直接退出子进程
        {
            if (errno == EINTR)
                continue;
            else //如果返回值<0,并且不是中断的信号=>退出进程,返回-1
                ERR_EXIT("read");
        }
        else 
        {
            // 打印接收到的数据到标准输出
            fputs(recv_buf,stdout);
            // 发送接收到的数据给对方
            write(conn_sockfd,recv_buf,sizeof(recv_buf));
        }
    }
    // (运行到这里只有可能是对方关闭了套接字)
    // 关闭连接套接字
    close(conn_sockfd);
}

int main()
{
    // 创建套接字,使用AF_UNIX,SOCK_STREAM,0.使用UNIX域中的流式套接字协议+错误处理
    int sockfd;
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0))<0)
        ERR_EXIT("socket");
    // (不需要设置地址重复利用)
    // 初始化地址结构
    struct sockaddr_un addr;
    memset(&addr,0,sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path,"test-socket");


    // 绑定ip
    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0) 
        ERR_EXIT("bind");



    // 使套接字处于监听状态
    if(listen(sockfd,SOMAXCONN)<0) // SOMAXCONN是服务器套接字允许建立的最大队列,包括未连接+已连接的队列
        ERR_EXIT("listen");
    // while(1)
    while(1)
    {
        // 接收对方的连接.NULL,NULL就行了+错误处理
        int conn_sockfd;
        if ((conn_sockfd = accept(sockfd,NULL,NULL))<0)
            ERR_EXIT("accept");
        // 创建1个子进程+错误处理
        pid_t pid;
        pid = fork();
        // 如果是子进程
        if (pid == 0)
        {
            // 关闭监听套接字
            close(sockfd);
            // 提供回射服务
            handle_echo_server(conn_sockfd);
            // 结束进程:正常
            exit(EXIT_SUCCESS);
        }
        // 如果是父进程
        else
        {
            // 关闭连接套接字
            close(conn_sockfd);
        
        }
    }
    // return 0
    return 0;

}
