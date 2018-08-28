#include <sys/types.h> 
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)


/*
 * 提供回射的客户端功能
 */
void handle_echo_client(int conn_sockfd)
{
    // 定义send_buf
    char send_buf[1024];
    // 定义recv_buf
    char recv_buf[1024];
    // while(读取键盘输入!=NULL)
    while(fgets(send_buf,sizeof(send_buf),stdin)!=NULL)
    {
    
        // 发送数据
        write(conn_sockfd,send_buf,sizeof(send_buf));
        // 接收数据+错误处理
        int ret = read(conn_sockfd,&recv_buf,sizeof(recv_buf)); // 接收到的数据是通过connection套接字来的.直接将这个套接字当成文件(因为是文件描述符)来处理,所以read/write就行了
        if (ret == 0) // 对方套接字如果关闭的话,就会返回0(表示发送过来的数据大小为0字节). 如果对方套接字关闭了,我们也结束死循环
        {
            printf("client close\n");
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
            // 打印数据
            fputs(recv_buf,stdout);
            // 清洗2个用户缓冲区
            memset(send_buf,0,sizeof(send_buf));
            memset(recv_buf,0,sizeof(recv_buf));
        }
    }
    // 关闭套接字
    close(conn_sockfd);
}


int main()
{

    // 创建1个套接字
    int sockfd;
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0))<0)
          ERR_EXIT("socket");
    // 创建服务器的地址(sockaddr_un,sun_path要和服务器的一样)
    struct sockaddr_un addr;
    memset(&addr,0,sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path,"test-socket");

    
    // 连接套接字到服务器+错误处理
    connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));
    // 提供回射的客户端功能
    handle_echo_client(sockfd);
    // return 0
    return 0;

}
