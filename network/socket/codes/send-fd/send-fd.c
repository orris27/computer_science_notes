#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

/* 
 * 发送文件描述符
 * 参数
 *  1. 发送给谁:sockfd
 *  2. 发送哪个套接字:sendfd
 */
void send_fd(int sockfd,int sendfd)
{
    // 定义发送的消息msg
    struct msghdr msg;
    /* 定义套接口的地址和地址长度 */
    // 1. 发送/接收套接口的地址=NULL
    msg.msg_name = NULL;
    // 2. 发送/接收套接口的地址长度=0
    msg.msg_namelen = 0;
    /* 定义I/O向量结构和长度 */
    // 1. 定义传递的数据,假设为1个字节,比如说0这个字符
    char sendchar = 0;
    // 2. 定义iovec结构体,iov_base指向数据缓冲区(即那个字节),iov_len就是数据缓冲区的字节长度
    struct iovec vec;
    vec.iov_base = &sendchar;
    vec.iov_len = sizeof(sendchar);
    // 3. 发送的消息的msg_iov指向iovec结构体
    msg.msg_iov = &vec;
    // 4. 定义I/O向量的长度为1(实际上是iovec结构体的个数)
    msg.msg_iovlen = 1;
    /* 定义辅助数据缓冲区的数组和长度 */
    // 1. 定义辅助数据缓冲区元素.char cmsgbuf[CMSG_SPACE(sizeof(sendfd))].缓冲区的字节长度通过CMSG_SPACE从发送的文件描述符大小中得出
    char cmsgbuf[CMSG_SPACE(sizeof(sendfd))];
    // 2. 赋值该缓冲区元素的地址(实际上就是cmsgbuf)给发送的消息msg的msg_control
    msg.msg_control = cmsgbuf;
    // 3. 赋值sizeof(cmsgbuf)给发送的消息msg的msg_controllen,因为msg_controllen是辅助缓冲区本身的大小,而不是辅助缓冲区的个数
    msg.msg_controllen = sizeof(cmsgbuf);
    // 4. 构造辅助缓冲区元素cmsgbuf
    // 4-1. 获取发送的消息msg的辅助数据缓冲区数组第1个元素的指针为p_cmsg
    struct cmsghdr *p_cmsg = CMSG_FIRSTHDR(&msg);
    // (p_cmsg=发送的消息msg的辅助数据缓冲区数组的第1个元素的指针.指向的数据类型为struct cmsghdr)
    // 4-2. 定义辅助数据缓冲区的大小.使用CMSG_LEN从发送的文件描述符中获得辅助数据缓冲区大小
    p_cmsg->cmsg_len = CMSG_LEN(sendfd);
    // 4-3. 定义使用的协议
    p_cmsg->cmsg_level = SOL_SOCKET;
    // 4-4. 定义传递的类型
    p_cmsg->cmsg_type = SCM_RIGHTS;
    // 4-5. 定义传递的文件描述符.使用CMSG_DATA从缓冲区指针从获得指向该属性的int指针,然后给这个int指针赋值sendfd
    int *p_fd = (int*)CMSG_DATA(p_cmsg);
    *p_fd = sendfd;
    /* 定义接收信息标记位 */
    // 标记接收信息标记位=0
    msg.msg_flags = 0;
    // 发送消息msg+错误处理
    int ret = sendmsg(sockfd,&msg,0);
    if(ret != 1)
        ERR_EXIT("sendmsg");
    
}
int recv_fd(const int sockfd)
{
    // 定义接收过来的文件描述符
    int recvfd;
    // 定义发送的消息msg
    struct msghdr msg;
    /* 定义套接口的地址和地址长度 */
    // 1. 发送/接收套接口的地址=NULL
    msg.msg_name = NULL;
    // 2. 发送/接收套接口的地址长度=0
    msg.msg_namelen = 0;
    /* 定义I/O向量结构和长度 */
    // 1. 定义接收的数据,假设为1个字节
    int recvchar;
    // 2. 定义iovec结构体,iov_base指向数据缓冲区(即那个字节),iov_len就是数据缓冲区的字节长度
    struct iovec vec;
    vec.iov_base = &recvchar;
    vec.iov_len = sizeof(recvchar);
    // 3. 发送的消息的msg_iov指向iovec结构体
    msg.msg_iov = &vec;
    // 4. 定义I/O向量的长度为1(实际上是iovec结构体的个数)
    msg.msg_iovlen = 1;
    /* 定义辅助数据缓冲区的数组和长度 */
    // 1. 定义辅助数据缓冲区元素.char cmsgbuf[CMSG_SPACE(sizeof(sendfd))].缓冲区的字节长度通过CMSG_SPACE从发送的文件描述符大小中得出
    char cmsgbuf[CMSG_SPACE(sizeof(recvfd))];
    // 2. 赋值该缓冲区元素的地址(实际上就是cmsgbuf)给发送的消息msg的msg_control
    msg.msg_control = cmsgbuf;
    // 3. 赋值sizeof(cmsgbuf)给发送的消息msg的msg_controllen,因为msg_controllen是辅助缓冲区本身的大小,而不是辅助缓冲区的个数
    msg.msg_controllen = sizeof(cmsgbuf);
    // 4. 构造辅助缓冲区元素cmsgbuf
    // 4-1. 获取发送的消息msg的辅助数据缓冲区数组第1个元素的指针为p_cmsg
    struct cmsghdr *p_cmsg = CMSG_FIRSTHDR(&msg);
    // (p_cmsg=发送的消息msg的辅助数据缓冲区数组的第1个元素的指针.指向的数据类型为struct cmsghdr)
    // 4-2. 定义传递的文件描述符.使用CMSG_DATA从缓冲区指针从获得指向该属性的int指针,然后给这个int指针赋值-1
    int *p_fd = (int*)CMSG_DATA(p_cmsg);
    *p_fd = -1;
    /* 定义接收信息标记位 */
    // 标记接收信息标记位=0
    msg.msg_flags = 0;
    // 接收消息msg+错误处理
    int ret = recvmsg(sockfd,&msg,0);
    if(ret != 1)
        ERR_EXIT("sendmsg");

    // 获取接收到消息msg的辅助数据缓冲区数组的第1个元素的指针+错误处理
    p_cmsg = CMSG_FIRSTHDR(&msg);
    if (p_cmsg == NULL)
        ERR_EXIT("no passed fd");
    

    // 使用int指针从辅助数据缓冲区元素的data部分读取文件描述符+错误处理
    p_fd = (int*)CMSG_DATA(p_cmsg);
    recvfd = *p_fd;
    if(recvfd == -1)
        ERR_EXIT("no passed fd");
    
    return recvfd;
}




int main()
{
    // 创建UNIX流协议的套接字对
    int sockets[2];
    if(socketpair(AF_UNIX,SOCK_STREAM,0,sockets) == -1)
          ERR_EXIT("socketpair");
        
    // 创建1个进程
    pid_t pid;
    if ((pid = fork()) == -1)
          ERR_EXIT("fork");

    // (假设子进程使用第1个套接字,而父进程使用第2个套接字)
    // 如果是子进程
    if (pid == 0)
    {
        // 关闭第2个套接字
        close(sockets[1]);
        // 定义1个文件描述符变量
        int fd;
        // 打开1个文件并传递给上述的文件描述符变量
        fd = open("test",'r');
        // 发送该文件描述符给父进程
        send_fd(sockets[0],fd);
    }
    // 如果是父进程
    else
    {
        // 关闭第1个套接字
        close(sockets[0]);
        // 定义1个文件描述符变量
        int fd;
        // 接收文件描述符
        fd = recv_fd(sockets[1]);
        // 定义用户缓冲区
        char buf[1024]; 
        // 读取文件内容到用户缓冲区中(读取文件描述符内的数据用read,不要用fgets)
        read(fd,buf,sizeof(buf));
        // 输出用户缓冲区的数据到标准输出
        fputs(buf,stdout);
    }

    // return 0
    return 0;
}
