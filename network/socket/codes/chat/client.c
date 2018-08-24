#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)


// 信号处理函数
//     退出
void handle_sigusr1(int sig_num)
{
    printf("receive signal %d=>keyboard process close\n",sig_num);
    exit(EXIT_SUCCESS);
}

void recv_socket(int conn_sockfd)
{

    // 定义接收数据的变量
    char recv_buf[1024];
    memset(recv_buf,0,sizeof(recv_buf));
    // 循环开始:通过连接套接字获取对方信息并保存到变量中
    while(1)
    {
        int ret = read(conn_sockfd,recv_buf,sizeof(recv_buf));
        // 如果获取失败,就退出进程
        if (ret == -1)
        {
            exit(EXIT_FAILURE);
        }

        // 如果获取大小为0,说明对方关闭了套接字,就退出循环
        if (ret == 0)
        {
            printf("socket process close\n");
            break;
        }
        // 如果获取成功,就输出对方信息
        fputs(recv_buf,stdout);
        // 清空变量自己
        memset(recv_buf,0,sizeof(recv_buf));
        // 循环结束
    }
    
    // 通知父进程结束自己
    kill(getppid(),SIGUSR1);
    // 子进程关闭连接套接字
    close(conn_sockfd);
    // 子进程也退出
    exit(EXIT_SUCCESS);

}


void recv_keyboard(int conn_sockfd)
{
    // 父进程定义信号处理函数,这样子进程可以发送信号过来叫父进程自己消灭自己
    signal(SIGUSR1,handle_sigusr1);

    // 定义接收键盘数据的变量
    char send_buf[1024];
    // 循环开始:接收键盘输入,并且存储到变量中
    while(fgets(send_buf,sizeof(send_buf),stdin)!=NULL)
    {
        // 将变量中的数据发送给对方套接字
        write(conn_sockfd,send_buf,strlen(send_buf));
        // 重新清空变量的内容
        memset(send_buf,0,sizeof(send_buf));
        // 循环结束
    }

    exit(EXIT_SUCCESS);
}


int main()
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




    pid_t pid;
    pid = fork();

    // 子进程接收对方套接字传递过来的数据
    if (pid == 0)
    {
        recv_socket(sockfd);
    }
    // 父进程接收键盘数据
    else
    {

        recv_keyboard(sockfd);
    }



    return 0;



    


    
    


}
