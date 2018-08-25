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

//定义数据包的格式=真实数据的字节长度+真实数据(字节流)
struct Packet {
    int len;
    char buf[BUFFER_LENGTH];
};

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
    struct Packet recv_buf;
    // 清洗接收数据变量
    memset(&recv_buf,0,sizeof(recv_buf));
    // 循环开始:通过连接套接字获取对方信息并保存到变量中
    while(1)
    {
        // 获取对方真实数据的长度
        int ret = readn(conn_sockfd,&recv_buf.len,4);
        // 如果获取失败,就退出进程
        if (ret == -1)
            exit(EXIT_FAILURE);
        // 如果获取大小=0,说明对方关闭了套接字,就退出循环
        else if (ret == 0)
        {
            printf("socket process close\n");
            break;
        }
        // 如果获取大小<4,说明出现异常,退出进程
        else if (ret< 4)
            exit(EXIT_FAILURE);
        // 字节序处理对方真实数据的长度
        recv_buf.len = ntohl(recv_buf.len);
        // 获取真实数据
        ret = readn(conn_sockfd,recv_buf.buf,recv_buf.len);
        // 如果获取失败,就退出进程
        if (ret == -1)
            exit(EXIT_FAILURE);
        // 获取大小不可能为0,因为上面已经处理过了
        // 获取大小!=包头里的长度,说明出现异常=>退出进程
        else if (ret != recv_buf.len)
            exit(EXIT_FAILURE);

        // 如果获取成功,就输出对方信息
        fputs(recv_buf.buf,stdout);
        // 清空变量自己
        memset(&recv_buf,0,sizeof(recv_buf));
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
    struct Packet send_buf;
    // 循环开始:接收键盘输入,并且存储到变量的真实数据成员中
    while(fgets(send_buf.buf,sizeof(send_buf.buf),stdin)!=NULL)
    {
        // 获取真实数据的长度
        int length = strlen(send_buf.buf);
        // 字节序处理数据的长度,并赋值到发送包的字节长度成员中
        send_buf.len = htonl(length);
        // 将自定义包发送给对方套接字
        writen(conn_sockfd,&send_buf,4+length);
        // 重新清空变量的内容
        memset(&send_buf,0,sizeof(send_buf));
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
