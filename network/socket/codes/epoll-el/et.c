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

int main()
{
    // 定义缓冲区的大小
    const int buf_size = 10;
    // 创建1个管道
    /* 定义保存读端/写端的2个套接字的数组 */
    int fds[2];
    /* 创建1个管道 */
    if((pipe(fds)) == -1)
          handle_error("pipe");
    /* 创建1个子进程 */
    pid_t pid;
    if ((pid = fork()) == -1)
            handle_error("fork");
    if (pid == 0) // 子进程
    {
        /* 关闭写端套接字(第2个套接字) */
        close(fds[1]);
    
        // 定义保存数据的缓冲区recv_buf
        char recv_buf[1024];
        /* 定义epoll红黑树(连接的套接字)的最大节点 */
        const int size = 10;
        /* 定义epoll的红黑树 */
        int epfd = epoll_create(size);
        if(epfd == -1)
              handle_error("epoll_create");
        /* 构造epoll红黑树节点struct epoll_event类型 */
        struct epoll_event event;
        //event.events = EPOLLIN;// 水平触发
        event.events = EPOLLIN|EPOLLET;// 边沿触发
        event.data.fd = fds[0];
        /* 添加监听套接字到红黑树 */
        if((epoll_ctl(epfd,EPOLL_CTL_ADD,fds[0],&event)) == -1)
              handle_error("epoll_ctl");
        // while(1)
        while(1)
        {
            /* 定义保存可读集合的数组 */
            struct epoll_event events[size];
            /* 等待epoll内的文件描述符的可读操作,并获得可操作的集合的个数nready+错误处理 */
            int nready = epoll_wait(epfd,events,size,-1);
            if (nready == -1)
            {
                if (errno == EINTR)
                    continue;
                else
                    handle_error("epoll_wait");
            }


            // (epoll红黑树节点只有1个,所以这里就不用判断了)
            // (执行到这里,说明读端套接字有可读操作)
            // 读取读端套接字里的5个字节到数据缓冲区
            read(fds[0],recv_buf,buf_size/2);
            // 打印数据
            write(STDOUT_FILENO,recv_buf,buf_size/2);
        }
    }
    else // 父进程
    {
        /* 关闭读端套接字(第1个套接字) */
        close(fds[0]);
        // 定义字符ch:初始值='a'
        char ch = 'a';
        // 定义写入用的缓冲区
        char buf[buf_size];
        // while(1)
        while(1)
        {
            // 写入缓冲区的前一半为ch
            for(int i=0;i<buf_size/2;++i)
                buf[i] = ch;
            // 修改缓冲区的前一半的最后一个为'\n'
            buf[buf_size/2-1] = '\n';
            // 更新字符变量ch:+1
            ch += 1;
            // 写入缓冲区的后一半为ch
            for(int i=buf_size/2;i<buf_size;++i)
                buf[i] = ch;
            // 修改缓冲区的后一半的最后一个为'\n'
            buf[buf_size-1] = '\n';
            // 更新字符变量ch:+1
            ch += 1;
            // 写入缓冲区的数据到写端套接字
            write(fds[1],buf,sizeof(buf));
            // 睡5s
            sleep(5);
        }
    
    }

    // 退出:0
    return 0;
}
