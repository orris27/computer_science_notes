#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

int main()
{
    // 定义接收套接字对的数组
    int sockets[2];
    // 创建套接字对
    if(socketpair(AF_UNIX,SOCK_STREAM,0,sockets) == -1)
        ERR_EXIT("socketpair");
    // 创建1个进程+错误处理
    pid_t pid;
    if ((pid = fork()) == -1)
        ERR_EXIT("fork");
    // (假定子进程使用第1个套接字,而父进程使用第2个套接字)
    // 如果是子进程[自增后发送数据过去,接收回来]
    if (pid == 0)
    {
        // 定义1个变量,初始化为0
        int var = 0;
        // 关闭第2个套接字
        close(sockets[1]);
        // while(1)
        while(1)
        {
            // 自增该变量
            ++var;
            // (不用转换成网络字节序,因为是本机内通信)
            // 打印说我们发送了什么数据
            printf("send:%d\n",var);
            // 写入变量内容到第1个套接字
            write(sockets[0],&var,sizeof(var));
            // 读取第1个套接字的数据,赋值给该变量
            read(sockets[0],&var,sizeof(var));
            // 打印该变量
            printf("recv:%d\n",var);
            // 睡1秒
            sleep(1);
        }
    }
    // 如果是父进程
    else
    {
        // 定义1个变量
        int var;
        // 关闭第1个套接字
        close(sockets[0]);
        // while(1)
        while(1)
        {
            // 读取第2个套接字的数据,赋值给该变量
            read(sockets[1],&var,sizeof(var));
            // 自增该变量
            ++var;
            // 写入变量内容到第2个套接字
            write(sockets[1],&var,sizeof(var));
        }
    }
    // return 0
    return 0;
}
