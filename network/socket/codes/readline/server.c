#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)


#define BUFFER_LENGTH 1024


/* 写入到文件描述符里的字节长度必须是count大小,返回值是已经读取的字节长度 */
ssize_t readn(int fd, void *buf, size_t count)
{
    //假定count值就是定长包的字节长度
    //定义剩余的字节数为变量,size_t nleft = count;
    size_t nleft = count;
    //定义指针bufp,指向buf,因为我们要移动接收
    char * bufp = (char*)buf;

    //开始循环接收,只要剩余的字节数>0就接收
    while(nleft > 0)
    {
        //读取文件描述符,写入到bufp指向的剩余字节长度的数据,使用read函数
        int ret = read(fd,bufp,nleft);
        //如果返回值<0,并且为中断=>继续读取
        if (ret < 0 && errno == EINTR)
            continue;
        //如果返回值<0,并且不是中断的信号=>退出进程,返回-1
        else if (ret < 0)
            return -1;
        //如果返回值为0,说明对方关闭套接字了(返回值是count,因为这是正常关闭)=>返回已经读取了的字节数
        else if ( ret == 0)
            return count - nleft;
        //移动指针
        bufp += ret;
        //更新剩余的字节数
        nleft -= ret;
        //结束循环
    }
    //返回定长的数值,count
    return count;
}

/* 写入到文件描述符里的字节长度必须是count大小,返回值是已经读取的字节长度 */
ssize_t writen(int fd, const void *buf, size_t count)
{
    //假定count值就是定长包的字节长度
    //定义剩余的字节数为变量,size_t nleft = count;
    size_t nleft = count;
    //定义指针bufp,指向buf,因为我们要移动发送
    char * bufp = (char*)buf;

    //开始循环发送,只要剩余的字节数>0就发送
    while(nleft > 0)
    {
        //写入到文件描述符中,内容是bufp指向的剩余字节长度的数据,使用write函数
        int ret = write(fd,bufp,nleft);
        //继续读取,如果返回值<0,并且为中断
        if (ret < 0 && errno == EINTR)
            continue;
        //退出进程,返回已经读取的字节长度,如果返回值<0,并且不是中断的信号
        else if (ret < 0)
            return count - nleft;
        //继续循环,如果返回值为0,说明什么都没有发送
        else if ( ret == 0)
            continue;
        //移动指针
        bufp += ret;
        //更新剩余的字节数
        nleft -= ret;
        //结束循环
    }
    //返回定长的数值,count
    return count;
}

ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    while(1)
    {
        //开始循环:获取套接口缓冲区里的数据,使用recv函数
        int ret = recv(sockfd,buf,len,MSG_PEEK);
        //  如果是中断的情况,就继续循环
        if (ret == -1 && errno == EINTR)
            continue;
        //  返回结果
        return ret;
    }
}


ssize_t readline(int sockfd, void *buf, size_t maxlen)
{
    //定义剩余字节长度
    int nleft = maxlen;
    //定义数据缓冲区的指针
    char *bufp = buf;
    //开始循环:peek获取套接口缓冲区里的数据缓冲区中
    while(1)
    {
        int ret = recv_peek(sockfd,bufp,nleft);
        //错误处理:
        //如果返回<0的话,说明异常=>退出进程
        if (ret < 0)
            exit(EXIT_FAILURE);
        //如果返回=0的话,说明对方关闭套接字=>退出函数,也返回0
        else if (ret == 0)
        {
            printf("client close\n");
            return 0;
        }
        //判断接收到的缓冲区里是否有\n
        //遍历缓冲区的每个字节,长度为读取到的字节长度
        int i;
        for (i=0;i<ret;i++)
        {
            //如果有\n的话
            if(bufp[i] == '\n')
            {
                //用readn函数读走套接口缓冲区(长度一直到\n为止,包括\n,因为下标有值所以知道长度)
                int ret = readn(sockfd,bufp,i+1);            
                //错误处理
                //如果read函数的返回值没有达到指定长度的话,说明有问题=>退出进程
                if (ret < i+1)
                    exit(EXIT_FAILURE);
                //退出函数
                return maxlen - nleft + ret;
            }
        }
    
        //如果没有\n的话
        if (i == ret)
        {
            //更新剩余字节长度
            nleft -= ret;
            //错误处理
            //如果剩余字节长度<0,说明超出最大字节范围=>异常退出进程
            if (nleft < 0)
                exit(EXIT_FAILURE);
            //就先读出目前字节长度到数据缓冲区的指针里
            readn(sockfd,bufp,ret);
            //移动数据缓冲区的指针
            bufp += ret;
        }
    }
    //返回读取的字节长度(最大字节长度-剩余字节长度)
    return maxlen - nleft;
}



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
    char recv_buf[BUFFER_LENGTH];
    // 清洗接收数据变量
    memset(recv_buf,0,sizeof(recv_buf));
    // 循环开始:通过连接套接字获取对方信息并保存到变量中
    while(1)
    {
        // 获取对方真实数据
        int ret = readline(conn_sockfd,recv_buf,sizeof(recv_buf));
        // 如果获取失败,就退出进程
        if (ret == -1)
            ERR_EXIT("readline");
        // 如果获取大小=0,说明对方关闭了套接字,就退出循环
        else if (ret == 0)
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
    char send_buf[BUFFER_LENGTH];
    // 循环开始:接收键盘输入,并且存储到变量的真实数据成员中
    while(fgets(send_buf,sizeof(send_buf),stdin)!=NULL)
    {
        // 将真实数据发送给对方套接字,注意长度一定是strlen,而不要+1,因为\n已经算进去了,strlen只是排除\0的情况.而且在对方readline中,如果我们+1的话,对方\n读完后面还有个\0的字符要读,这样recv/readline就不会堵塞了=>造成问题
        writen(conn_sockfd,send_buf,strlen(send_buf));
        // 重新清空变量的内容
        memset(send_buf,0,sizeof(send_buf));
        // 循环结束
    }

    exit(EXIT_SUCCESS);
}

int main()
{
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


    if ((conn_sockfd = accept(sockfd,(struct sockaddr*)&peer_addr,&peer_len))<0)
      ERR_EXIT("accept");

    // 打印对方的ip地址和端口号
    printf("peer=%s:%d\n",inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));

    // 关闭监听套接字,因为2个进程都不需要了
    close(sockfd);
    pid_t pid;
    pid = fork();

    // 子进程接收对方套接字传递过来的数据
    if (pid == 0)
    {
        recv_socket(conn_sockfd);
    }
    // 父进程接收键盘数据
    else
    {

        recv_keyboard(conn_sockfd);
    }

    // close all the sockets
    close(conn_sockfd);
    return 0;
}
