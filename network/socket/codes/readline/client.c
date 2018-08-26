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
        // 将真实数据发送给对方套接字
        writen(conn_sockfd,send_buf,strlen(send_buf));
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
