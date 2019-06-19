## 1. 网络编程
1. [回射C/S-TCP模型](https://github.com/orris27/orris/tree/master/network/socket/codes/simple-tcp).[图解](https://ask.qcloudimg.com/http-save/yehe-1147827/sams3iwj3k.png?imageView2/2/w/1620)
2. 创建ipv4地址
    1. 方法1:(不能用在局域网)
    ```
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188); 
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // (struct sockaddr*)&addr // 转换成通用地址结构体的指针,供connect等函数使用
    ```
    2. 方法2:(能用在局域网)
    ```
    struct sockaddr_in addr;
    
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0" , &addr.sin_addr.s_addr);
    /*addr.sin_addr.s_addr = htonl(INADDR_ANY);*/
    addr.sin_port = htons(5188);
    ```
3. 发送数据
    1. write(UNIX的流协议/TCP/socketpair下的UNIX流协议)
    ```
    char send_buf[1024];
    /* fgets(send_buf,sizeof(send_buf),stdin); */
    if((write(sockfd,send_buf,sizeof(send_buf))) == -1) //直接网套接字里写数据就行了
        handle_error("write");
    
    ```
    2. UDP的sendto函数(无连接)
        + sockfd
        + send_buf
        + addr(struct sockaddr_in,对方的ip地址)
    ```
    sendto(sockfd,send_buf,strlen(send_buf),0,(struct sockaddr*)&addr,sizeof(addr));
    
    ```
    3. UDP的sendto函数(有连接)
        + sockfd
        + send_buf
        + addr(struct sockaddr_in,对方的ip地址)
    ```
    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0)
        handle_error("connect");
        
    sendto(sockfd,send_buf,strlen(send_buf),0,NULL,0);
    ```

4. 接收数据
    1. UNIX流协议/TCP接收数据/socketpair下的UNIX流协议
    ```
    char recv_buf[1024];
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
            handle_error("read");
    }
    ```
    2. UDP接收数据
    ```
    int ret = recvfrom(sockfd,recv_buf,sizeof(recv_buf),0,NULL,0);
    if (ret == -1)// 如果read出现错误,就直接退出子进程
    {
        if (errno == EINTR)
            continue;   
        else //如果返回值<0,并且不是中断的信号=>退出进程,返回-1
            handle_error("read");
    }
    else if(ret > 0)
    { // 如果返回正确=>我们怎么处理
        
    }

    ```
    
5. 打印ip和port
    + 套接字里的sin_port和sin_addr都是网络字节序.
```
printf("peer=%s:%d\n",inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));
```
6. 检测TCP客户端套接字是否关闭(read返回值表示套接字传送过来的数据大小,如果为0说明就关闭了)
```
int ret = read(conn_sockfd,&recv_buf,sizeof(recv_buf)); 
if (ret == 0)
{
    printf("client close\n");
    break;
}
```
7. [点对点聊天程序](https://github.com/orris27/orris/tree/master/network/socket/codes/chat)
8. [解决TCP粘包问题-自定义包头](https://github.com/orris27/orris/tree/master/network/socket/codes/fixed-length)
9. [解决TCP粘包问题-\n结尾](https://github.com/orris27/orris/tree/master/network/socket/codes/readline)
10. 创建套接字
    1. TCP套接字
    ```
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
      handle_error("socket");
    
    if((close(sockfd)) == -1)
        handle_error("close");

    ```
    2. UDP套接字
    ```
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)
      handle_error("socket");
      
    if((close(sockfd)) == -1)
        handle_error("close");

    ```
    3. UNIX套接字
    ```
    int sockfd;
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0))<0)
        handle_error("socket");
        
    if((close(sockfd)) == -1)
        handle_error("close");

    ```
    4. 创建UDP套接字并绑定ip地址
    ```
    int sockfd;
    struct sockaddr_in addr;
    
    if ((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)
        handle_error("socket");
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188);
    /*addr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
    inet_pton(AF_INET, "0.0.0.0" , &addr.sin_addr.s_addr);
    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0) 
        handle_error("bind");
        
    if((close(sockfd)) == -1)
        handle_error("close");

    ```
    
    5. 创建TCP套接字并根据ip地址连接
    ```
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
      handle_error("socket");

    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188);
    /*addr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
    inet_pton(AF_INET, "0.0.0.0" , &addr.sin_addr.s_addr);

    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0)
      handle_error("connect");
      
      
    if((close(sockfd)) == -1)
        handle_error("close");

    ```
    6. 创建UNIX套接字,并删除原来的套接字文件,并绑定UNIX地址,并处于监听状态
    ```
    int sockfd;
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0))<0)
        handle_error("socket");

    struct sockaddr_un addr;
    memset(&addr,0,sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path,"/tmp/test-socket");

    unlink(addr.sun_path);
    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0)
        handle_error("bind");
    
    if(listen(sockfd,SOMAXCONN)<0)
        handle_error("listen");
        
        
    if((close(sockfd)) == -1)
        handle_error("close");

    ```
    7. 创建UNIX套接字,并连接UNIX地址
    ```
    int sockfd;
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0))<0)
          handle_error("socket");

    struct sockaddr_un addr;
    memset(&addr,0,sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path,"/tmp/test-socket");
    
    /* 连接套接字到服务器+错误处理 */
    connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));

    if((close(sockfd)) == -1)
        handle_error("close");

    ```
    8. 创建套接字对(socketpair)
    ```
    int sockets[2];
    if(socketpair(AF_UNIX,SOCK_STREAM,0,sockets) == -1)
        handle_error("socketpair");
        
    
    if((close(sockets[0])) == -1)
        handle_error("close");
    if((close(sockets[1])) == -1)
        handle_error("close");

    ```
    9. 创建UNIX流协议的套接字对,并创建1个进程,父子进程各使用其中1个套接字
    ```
    int sockets[2];
    if(socketpair(AF_UNIX,SOCK_STREAM,0,sockets) == -1)
        handle_error("socketpair");
        
    pid_t pid;
    if ((pid = fork()) == -1)
        handle_error("fork");
    
    if (pid == 0) // (假定子进程使用第1个套接字,而父进程使用第2个套接字)
    {
        close(sockets[1]);
        if((close(sockets[1])) == -1)
            handle_error("close");
        



        if((close(sockets[0])) == -1)
            handle_error("close");
    }
    else
    {
        close(sockets[0]);
        
        
        
        
        if((close(sockets[1])) == -1)
            handle_error("close");
    }
    ```

11. 获取套接字地址
    1. 获取自身套接字地址
    ```
    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    if (getsockname(sockfd,(struct sockaddr*)&localaddr,&addrlen) < 0)
        handle_error("getsockname");
    ```
    2. 获取对方套接字地址(该套接字必须处于连接状态)
    ```
    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    if (getpeername(sockfd,(struct sockaddr*)&localaddr,&addrlen) < 0)
        handle_error("getpeername");
    ```

12. 使UNIX流协议/TCP套接字处于监听状态
```
if(listen(sockfd,SOMAXCONN)<0) // SOMAXCONN是服务器套接字允许建立的最大队列,包括未连接+已连接的队列
  handle_error("listen");
```

13. 设置套接字属性
    1. 允许服务器套接字重用TIME_WAIT套接字(实验中使用在bind函数之前,具体怎样不太清楚)
    ```
    int reuse_on = 1;
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse_on,sizeof(reuse_on))<0)
      handle_error("setsockopt");
    ```
    2. 允许套接字发送广播
    ```
    int broadcast_on = 1;
    if(setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&broadcast_on,sizeof(broadcast_on))<0)
      handle_error("setsockopt");
    ```
14. [使用poll函数实现并发服务器](https://github.com/orris27/orris/tree/master/network/socket/codes/echo)

15. 字节序和点分ip转换
```
unsigned int inip;
inip = inet_addr("192.168.0.100"); //点分ip=>网络字节序的ip
printf("addr=%u\n",ntohl(inip));  // 网络字节序的ip=>主机字节序的ip

struct in_addr ipaddr;
ipaddr.s_addr = inip;
printf("%s\n",inet_ntoa(ipaddr)); // 网络字节序ip=>点分ip
```


16. 接收对方连接,并获取对方地址信息
```
struct sockaddr_in peer_addr;
socklen_t peer_len = sizeof(peer_addr); // 使用accept获得对方的套接字等信息时,对方套接字的长度一定要初始化
int conn_sockfd;
if ((conn_sockfd = accept(sockfd,(struct sockaddr*)&peer_addr,&peer_len))<0)
  handle_error("accept");
```
17. 获取主机名
```
char hostname[100]={0};
if (gethostname(hostname,sizeof(host) <0)
    handle_error("gethostname");
```

18. 获取本机的所有ip列表(通过主机名获取)(虚拟机能获取`10.0.0.7`,但是获取不了`127.0.0.1`,我自己的电脑只能获取到`127.0.1.1`)
```
// 获取主机名
char hostname[100]={0};
if(gethostname(hostname,sizeof(hostname)) < 0)
    handle_error("gethostname");


// 获取保存ip列表的结构体,通过主机名
struct hostent *hp;
if ((hp = gethostbyname(hostname)) == NULL)
    handle_error("gethostbyname");


// 遍历该主机下的ip列表,每个ip元素存储的都是ip的结构体,可以强制类型转换成struct in_addr*类型
int i = 0;
while (hp->h_addr_list[i] != NULL)
{

    // 打印出ip地址信息,通过ip地址的结构体
    printf("%s\n",inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]));
    // i++
    i++;
}
```



19. 获取本机IP地址(我的虚拟机里获取出来正确,但是我的电脑本身却只能获得`127.0.1.1`)
```
int getlocalip(char *ip)
{
    char hostname[100]={0};
    if(gethostname(hostname,sizeof(hostname)) < 0)
        return -1;


    // 获取保存ip列表的结构体,通过主机名
    struct hostent *hp;
    if ((hp = gethostbyname(hostname)) == NULL)
        return -1;

    // 打印出ip地址信息,通过ip地址的结构体
    strcpy(ip,inet_ntoa(*(struct in_addr*)hp->h_addr)); // man gethostbyname中有定义#define h_addr h_addr_list[0]
    return 0;
}


// ....
char localip[16] = {0};
if (getlocalip(localip) == -1)
    handle_error("getlocalip");
printf("%s\n",localip);

```



20. 解决僵尸进程
    1. (方法1)在服务器端的main函数里添加`signal(SIGCHLD,SIG_IGN);`就好了
    2. (方法2)捕捉SIGCHLD信号,调用wait函数=>问题:只能等待一个子进程退出
        1. 实现
        ```
        void handle_sigchld(int sig)
        {   
            //调用wait函数子进程的状态
            wait(NULL);
            //wait进程仅仅等待第一个子进程的退出
        }
        if((signal(SIGCHLD,handle_sigchld)) == SIG_ERR)
            handle_error("signal");
        ```
        2. 问题:不能关闭所有子进程的问题重现<=多个信号SIGCHLD同时发送给服务端的时候,由于这些信号是不可靠信号,并且只能排队一个,所以就会丢失信号(当然信号的到来不一定是同时的,那么就会看到用wait可以消除多个僵尸进程的情况)
        ```
        ./server
        ./multiclient # 如果没有观察到的话,可以考虑把同时连接的套接字的个数再增加.
        #------------------------------------------------------------------
        # orris    16530  5904  0 21:32 pts/1    00:00:00 ./server
        # orris    16550 16530  0 21:32 pts/1    00:00:00 [server] <defunct>
        # orris    16551 16530  0 21:32 pts/1    00:00:00 [server] <defunct>
        #------------------------------------------------------------------

        ```
    3. (方法3,推荐)捕捉到SIGCHLD信号后,轮询所有子进程,如果发现是退出状态,就调用waitpid执行,从而即使只收到1个SIGCHLD,也会遍历到要退出的子进程
    ```
    void handle_sigchld(int sig)
    {
        //循环调用waitpid去轮询子进程,如果检测到他们是退出状态,我们就释放该僵尸进程
        while(waitpid(-1,NULL,WNOHANG)>0);
    }

    int main()
    // ...
    
    if((signal(SIGCHLD,handle_sigchld)) == SIG_ERR)
        handle_error("signal");
    
    ```
    4. 获取子进程退出信息
    ```
    // 定义接收子进程退出值的变量
    int status;
    // 回收子进程
    if((wait(&status)) == -1)
        handle_error("wait");
    // 如果子进程是正常退出
    if(WIFEXITED(status))
    {
        // 打印子进程的退出状态
        printf("child exits with %d\n",WEXITSTATUS(status));
    }
    // 如果子进程是异常退出
    else if(WIFSIGNALED(status))
    {
        // 打印导致子进程异常退出的信号
        printf("child killed by %d\n",WTERMSIG(status));
    }

    ```
21. [重现FIN_WAIT2和CLOSE_WAIT](https://github.com/orris27/orris/tree/master/network/socket/codes/echo)
22. [解决SIGPIPE信号](https://github.com/orris27/orris/tree/master/network/socket/codes/echo)
23. [使用select解决CLOSE_WATI和FIN_WAIT2状态](https://github.com/orris27/orris/tree/master/network/socket/codes/echo)
24. [单进程select服务器](https://github.com/orris27/orris/tree/master/network/socket/codes/echo)
25. [回射服务器中使用shutdown解决关闭后不能读取的问题](https://github.com/orris27/orris/tree/master/network/socket/codes/echo)
```
shutdown(sockfd,SHUT_WR);
```
26. 获取文件描述符大小(我这边cur=1024,而`ulimit -n`的值=1024,而rlim_max为4096.使用程序测试后发现1024是极限)
```
struct rlimit rl;
if (getrlimit(RLIMIT_NOFILE,&rl) <0)
      handle_error("getrlimit");
printf("%d\n",(int)rl.rlim_cur); 
printf("%d\n",(int)rl.rlim_max);
```
27. 修改文件描述符(调成2048后,实际打开的文件在1151就停止住了..<=我使用的select服务器,是select受到FD_SETSIZE限制.使用poll服务器后,客户端和服务器都修改后就正确了=>该代码可行的)
```
//修改当前进程的文件描述符
struct rlimit rl;
rl.rlim_cur = 2048;
rl.rlim_max = 2048;
if (setrlimit(RLIMIT_NOFILE,&rl) <0)
      handle_error("getrlimit");

//查看当前进程的文件描述符
if (getrlimit(RLIMIT_NOFILE,&rl) <0)
      handle_error("getrlimit");
printf("%d\n",(int)rl.rlim_cur);
printf("%d\n",(int)rl.rlim_max);
```


28. err_exit
```
#define handle_error(m) do { perror(m); exit(EXIT_FAILURE); } while(0)
```

29. 网络编程包含的头文件
```
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
```

30. Makefile: 参考[Makefile的notes](https://github.com/orris27/orris/blob/master/cpp/makefile.md)
    1. 原始形态(produce有main函数,produce依赖fifo,而fifo依赖sem,其中fifo和sem都有头文件)
        + `gcc -c xx.c -o xx.o`:编译
        + `gcc xx.o yy.o -o zz`:连接
    ```
    produce:produce.o fifo.o sem.o
        gcc produce.o fifo.o sem.o -o produce
    sem.o:sem.c
        gcc -c sem.c -o sem.o
    fifo.o:fifo.c
        gcc -c fifo.c -o fifo.o
    produce.o:produce.c
        gcc -c produce.c -o produce.o
    clean:
        rm -f *.o produce fifo sem consume
    ```
    2. 使用(下面的4个空格可能要换成tab键)
        1. 在BIN中写入要编译并且含有main函数的c文件的文件名(不包括`.c`,比如说`client.c`就写成`BIN=client`)
        2. `make`
    ```
    .PHONY:clean all
    CC=gcc
    CFLAGS=-Wall -g
    BIN=
    all:$(BIN)
    $.o:%.c
        $(CC) $(CFLAGS) -c $< -o $@
    clean:
        rm -f *.o $(BIN)
    ```
    3. 多个文件的Makefile
        1. 说明
            + produce(main)=>fifo=>sem
            + consume(main)=>fifo=>sem
        2. 注意
            1. BIN:只存放含有main函数的c文件.(没有main函数的只需要在OBJS中指明就会被自动编译)
            2. 每个OBJS对应1个需要连接的BIN文件,并在下面输出
    ```
    .PHONY:clean all
    CC=gcc
    CFLAGS=-Wall -g
    BIN=produce consume
    OBJS1=produce.o fifo.o sem.o
    OBJS2=consume.o fifo.o sem.o
    all:$(BIN)
    $.o:%.c
        $(CC) $(CFLAGS) -c $< -o $@
    produce:$(OBJS1)
        $(CC) $(CFLAGS) $^ -o $@
    consume:$(OBJS2)
        $(CC) $(CFLAGS) $^ -o $@
    clean:
        rm -f *.o $(BIN)
    #------------------------------------------------------------------------
    # gcc -Wall -g   -c -o produce.o produce.c
    # gcc -Wall -g   -c -o fifo.o fifo.c
    # gcc -Wall -g   -c -o sem.o sem.c
    # gcc -Wall -g produce.o fifo.o sem.o -o produce
    # gcc -Wall -g   -c -o consume.o consume.c
    # gcc -Wall -g consume.o fifo.o sem.o -o consume
    #------------------------------------------------------------------------
    ```
    4. 连接时添加`-lrt`参数(POSIX需要在连接时添加额外参数)
        1. 显示写出连接过程,直接写在后面就行了
        2. `$^`:可能是后者,`$@`:可能是前者
    ```
    .PHONY:clean all
    CC=gcc
    CFLAGS=-Wall -g
    BIN=mq
    all:$(BIN)
    $.o:%.c
        $(CC) $(CFLAGS) -c $< -o $@
    mq:mq.o
        $(CC) $(CFLAGS) $^ -o $@ -lrt
    clean:
        rm -f *.o $(BIN)
    ```

31. 循环读取键盘输入
```
while(fgets(send_buf,sizeof(send_buf),stdin) != NULL)
{

}
```
32. 创建进程
    1. 创建1个子进程
    ```
    pid_t pid;
    if ((pid = fork()) == -1)
        handle_error("fork");
    if (pid == 0) // 子进程
    {
    }
    else // 父进程
    {
    }
    ```
    2. 创建4个子进程,并用no变量做编号,其中父进程为0,后续的子进程从1开始编号
    ```
    int no = 0;
    for(int i=0;i<4;++i)
    {
        pid_t pid;
        if ((pid = fork()) == -1)
              handle_error("fork");
        if (pid == 0) // 子进程
        {
            no = i+1; // 设置no=i
            break;
        }
    }

    ```
33. [socketpair实现发送数据给父/子进程让对方来自增变量](https://github.com/orris27/orris/tree/master/network/socket/codes/socketpair)

34. 传递文件描述符.(传递文件描述符程序实现)[https://github.com/orris27/orris/tree/master/network/socket/codes/send-fd]
    1. 传递文件描述符
    ```
    void send_fd(int sockfd,int sendfd) // 参数:传递给谁=>sockfd;传递哪个套接字=>sendfd
    {
        struct msghdr msg;
        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        char sendchar = 0;
        struct iovec vec;
        vec.iov_base = &sendchar;
        vec.iov_len = sizeof(sendchar);
        msg.msg_iov = &vec;
        msg.msg_iovlen = 1;
        char cmsgbuf[CMSG_SPACE(sizeof(sendfd))];
        msg.msg_control = cmsgbuf;
        msg.msg_controllen = sizeof(cmsgbuf);
        struct cmsghdr *p_cmsg = CMSG_FIRSTHDR(&msg);
        p_cmsg->cmsg_len = CMSG_LEN(sendfd);
        p_cmsg->cmsg_level = SOL_SOCKET;
        p_cmsg->cmsg_type = SCM_RIGHTS;
        int *p_fd = (int*)CMSG_DATA(p_cmsg);
        *p_fd = sendfd;
        msg.msg_flags = 0;
        int ret = sendmsg(sockfd,&msg,0);
        if(ret != 1)
            handle_error("sendmsg");

    }

    ```
    2. 接收文件描述符
    ```
    int recv_fd(const int sockfd)//参数:从哪里接收套接字=>sockfd.返回值:接收过来的套接字
    {
        int recvfd;
        struct msghdr msg;
        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        int recvchar;
        struct iovec vec;
        vec.iov_base = &recvchar;
        vec.iov_len = sizeof(recvchar);
        msg.msg_iov = &vec;
        msg.msg_iovlen = 1;
        char cmsgbuf[CMSG_SPACE(sizeof(recvfd))];
        msg.msg_control = cmsgbuf;
        msg.msg_controllen = sizeof(cmsgbuf);
        struct cmsghdr *p_cmsg = CMSG_FIRSTHDR(&msg);
        int *p_fd = (int*)CMSG_DATA(p_cmsg);
        *p_fd = -1;
        msg.msg_flags = 0;
        int ret = recvmsg(sockfd,&msg,0);
        if(ret != 1)
            handle_error("sendmsg");

        p_cmsg = CMSG_FIRSTHDR(&msg);
        if (p_cmsg == NULL)
            handle_error("no passed fd");


        p_fd = (int*)CMSG_DATA(p_cmsg);
        recvfd = *p_fd;
        if(recvfd == -1)
            handle_error("no passed fd");

        return recvfd;
    }
    ```

35. 定长read和write操作,可以解决TCP粘包问题.[具体实现](https://github.com/orris27/orris/tree/master/network/socket/codes/fixed-length)
```
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
```
36. 读取一行
```
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

```

37. epoll
    1. 定义epoll红黑树
    ```
    /* 定义epoll红黑树(连接的套接字)的最大节点 */
    const int size = 10;
    /* 定义epoll的红黑树 */
    int epfd = epoll_create(size);
    if(epfd == -1)
        handle_error("epoll_create");
    ```
    2. 添加监听套接字到epoll红黑树中
        1. 水平触发
        ```
        /* 构造epoll红黑树节点struct epoll_event类型 */
        struct epoll_event event;
        event.events = EPOLLIN;// 水平触发
        /*event.events = EPOLLIN|EPOLLET;// 边沿触发*/
        event.data.fd = sockfd;
        /* 添加监听套接字到红黑树 */
        if((epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&event)) == -1)
            handle_error("epoll_ctl");
        ```
        2. 边沿触发+非阻塞IO
        ```
        /* 构造epoll红黑树节点struct epoll_event类型 */
        struct epoll_event event;
        event.events = EPOLLIN|EPOLLET;
        event.data.fd = sockfd;
        /* 修改套接字的属性:非阻塞IO */
        int flag = fcntl(sockfd,F_GETFL);
        if(flag == -1)
            handle_error("fcntl");
        flag |= O_NONBLOCK;
        fcntl(sockfd,F_SETFL,flag);
        /* 添加监听套接字到红黑树 */
        if((epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&event)) == -1)
            handle_error("epoll_ctl");

        ```
    3. 等待epoll的事件
    ```
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

    /* 遍历集合.for(i:0=>nready) */
    for(int i=0;i<nready;++i)
    {
        /*
         * ~~~~
         * // 过滤掉不是可读的事件
         * if(!(events[i].events & EPOLLIN))
         *     continue;
         * // 如果是监听套接字
         * if(events[i].data.fd == sockfd)
         * {
         * }
         * 
         * 
         */

    }
    ```
    4. 删除epoll红黑树中的节点
    ```
    /* 删除epoll红黑树里的连接套接字 */
    if((epoll_ctl(epfd,EPOLL_CTL_DEL,conn_sockfd,NULL)) == -1)
        handle_error("epoll_ctl");
    ```
    5. [epoll的水平触发和边沿触发基于管道的比较实现](https://github.com/orris27/orris/tree/master/network/socket/codes/epoll-el)
    6. [epoll反应堆实现](https://github.com/orris27/orris/tree/master/network/socket/codes/epoll-reactor)


38. [UDP广播实现](https://github.com/orris27/orris/tree/master/network/socket/codes/broadcast)
39. 组播
    1. 允许套接字能发送组播信息
    ```
    /* 构造组信息 */
    struct ip_mreqn group;
    group.imr_multiaddr.s_addr = inet_addr("239.0.0.2");
    group.imr_address.s_addr = inet_addr("0.0.0.0");
    group.imr_ifindex = if_nametoindex("eth0");

    /* 允许套接字能发送组播消息:参数=组信息 */
    if(setsockopt(sockfd,IPPROTO_IP,IP_MULTICAST_IF,&group,sizeof(group))<0)
        handle_error("setsockopt");

    ```
    2. 加入套接字得到组播地址
    ```
    /* 构造组信息 */
    struct ip_mreqn group;
    group.imr_multiaddr.s_addr = inet_addr("239.0.0.2");
    group.imr_address.s_addr = inet_addr("0.0.0.0");
    group.imr_ifindex = if_nametoindex("eth0");

    /* 允许套接字能发送组播消息:参数=组信息 */
    if(setsockopt(sockfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&group,sizeof(group))<0)
        handle_error("setsockopt");
    ```
    3. 给组播地址发送信息
    ```
    // 给"239.0.0.2"发送信息就行了
    inet_pton(AF_INET, "239.0.0.2" , &peer_addr.sin_addr.s_addr);
    ```
    1. [UDP组播实现](https://github.com/orris27/orris/tree/master/network/socket/codes/multicast)

40. 计算UNIX的struct sockaddr_un的实际大小
```
int len = offsetof(struct sockaddr_un,sun_path) + strlen(addr.sun_path)
```

## 2. IPC
### 2-1. System V
#### 2-1-1. 消息队列

1. 创建key为1234消息队列
```
int msgid = msgget(1234,0666|IPC_CREAT);
if(msgid == -1)
    handle_error("msgget");
```
2. 打开key为1234的消息队列
```
int msgid = msgget(1234,0);
if(msgid == -1)
    handle_error("msgget");
```
3. 删除消息队列
```
int msgid = msgget(1234,0);
if(msgid == -1)
    handle_error("msgget");
    
if((msgctl(msgid,IPC_RMID,NULL)) == -1)
    handle_error("msgctl");
```
4. 获得消息队列的状态,并输出它的信息
```
int msgid = msgget(1234,0);
if(msgid == -1)
      handle_error("msgget");

struct msqid_ds msg_stat;
if((msgctl(msgid,IPC_STAT,&msg_stat) == -1))
    handle_error("msgctl");

printf("mode=%o\n",msg_stat.msg_perm.mode);//输出消息队列的权限

printf("number=%d\n",(int)msg_stat.msg_qnum);//输出消息队列中消息的个数

printf("bytes=%ld\n",msg_stat.__msg_cbytes);//输出消息队列中所有消息的实际数据的字节数和

printf("msgmnb=%d\n",(int)msg_stat.msg_qbytes);//输出消息队列中所有消息的实际数据的字节数和的最大值
```
5. 设置消息队列的状态,比如说修改消息队列的权限
```
int msgid = msgget(1234,0);
if(msgid == -1)
      handle_error("msgget");

struct msqid_ds msg_stat;
if((msgctl(msgid,IPC_STAT,&msg_stat) == -1))
    handle_error("msgctl");

sscanf("600","%o",(unsigned int*)&msg_stat.msg_perm.mode);
if((msgctl(msgid,IPC_SET,&msg_stat) == -1))
    handle_error("msgctl");
```

6. 发送消息
    1. 介绍
        1. 定义消息结构体
        2. bytes:发送的实际数据的字节数.可以用MSGMAX
        3. type:发送的消息类型
        4. 消息队列的key
    2. 简洁用法
    ```
    struct msgbuf *p_buf = (struct msgbuf *)malloc(sizeof(long) + bytes);
    p_buf->mtype = type;
    if((msgsnd(msgid,p_buf,MSGMAX,0)) == -1)
      handle_error("msgsnd");
    ```
    3. 完整用法
    ```
    struct msgbuf {
        long mtype;    
        char mtext[MSGMAX];  //man中提供的是1
    };

    // ...
    if(argc != 3)
    {
        fprintf(stderr,"Usage: %s <bytes> <type>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    int msgid = msgget(1234,0);
    if(msgid == -1)
          handle_error("msgget");
    int bytes = atoi(argv[1]); // 发送的实际数据的字节数
    int type  = atoi(argv[2]); // 发送的消息类型
    
    struct msgbuf *p_buf = (struct msgbuf *)malloc(sizeof(long) + bytes);
    p_buf->mtype = type;
    if((msgsnd(msgid,p_buf,bytes,0)) == -1) //if((msgsnd(msgid,p_buf,bytes,IPC_NOWAIT)) == -1)
        handle_error("msgsnd");

    ```
7. 接收key为1234的消息队列的消息
    1. 参数
        1. mtype:接收哪个消息类型
        2. msgflg:接收消息的函数的使用选项
    2. 简洁用法
    ```
    if ((msgrcv(msgid,p_buf,MSGMAX,mtype,0)) == -1)
      handle_error("msgrcv");
    ```
    3. 完整用法
    ```
    #define MSGMAX  8192
    struct msgbuf {
        long mtype;
        char mtext[MSGMAX]; //man中提供的是1
    };


    int msgid = msgget(1234,0);
    if(msgid == -1)
          handle_error("msgget");
    struct msgbuf *p_buf = (struct msgbuf *)malloc(sizeof(long) + MSGMAX);

    if ((msgrcv(msgid,p_buf,MSGMAX,mtype,msgflg)) == -1)
        handle_error("msgrcv");

    ```

8. 构造System V中消息队列中的1个消息
    1. 参数
        + bytes(MSGMAX):字节数
        + type:消息类型
```
struct msgbuf {
    long mtype;    
    char mtext[MSGMAX]; //man中提供的是1
};

struct msgbuf *p_buf = (struct msgbuf *)malloc(sizeof(long) + MSGMAX);
p_buf->mtype = mtype;

```

9. 设置命令行参数(getopt),比如说使用`msg-recv -n -t xx`中`-n`表示NOWAIT,而`-t`执行消息类型
    + 返回值是整型.如果是'?'表示新的参数,返回值是-1表示没有参数
```
int opt;
while(1)
{
    opt = getopt(argc,argv,"nt:"); //"n"表示-n且没有值,"t:"表示-t且有值
    if (opt == '?') // 出现新的参数.这里不使用handle_error是因为出现了新的参数也表示成功
        exit(EXIT_SUCCESS);
    if (opt == -1) // 如果没有命令行参数
        break;
    switch(opt)
    {
        
        case 'n': // 如果是'n'
            printf("nnnnn\n");
            break;
        case 't':
            
            printf("ttttt\n"); // 输出"ttttt"
            int val = atoi(optarg);
            printf("val=%d\n",val);
            break;

    }
}
```
10. 在缓冲区中的前4个字节里保存pid
```
char buf[1024];
int pid = getpid();
*(int*)buf = pid;
```
#### 2-1-2. 共享内存
1. Linux下打开/创建文件
    1. 以读写方式打开/创建文件,并清空文件,设置权限为0666
        + open得到文件描述符,而fopen得到FILE指针
    ```
    int fd = open(argv[1],O_CREAT|O_RDWR|O_TRUNC,0666);
    if(fd == -1)
        handle_error("open");
    /* (!!!非常重要)修改目标文件大小:大小=size */
    if((ftruncate(fd,size)) == -1)
          handle_error("ftruncate");

    if((close(fd)) == -1)
        handle_error("close");
    ```
    2. 打开文件
    ```
    int fd = open("filename",O_RDWR,0);
    if(fd == -1)
        handle_error("open");
    /* (!!!非常重要)修改目标文件大小:大小=size */
    if((ftruncate(fd,size)) == -1)
          handle_error("ftruncate");

    if((close(fd)) == -1)
        handle_error("close");
    ```
2. 移动文件描述符
```
lseek(fd,sizeof(Student)*5-1,SEEK_SET); // 移动到Student结构体大小的5倍-1的位置
```
3. 填充5个Student结构体大小的空间,并设置为0
```
lseek(fd,sizeof(Student)*5-1,SEEK_SET);
if((write(fd,"",1)) == -1)
    handle_error("write");
```
4. 映射文件到共享内存区
    + 注意:打开文件和映射文件的权限要对应.如果映射的参数是PROT_WRITE和MAP_SHARED的话,映射的实体文件需要O_RDWR的方式打开.参考`man mmap`的`EACCES`错误\
    
```
typedef struct{
    char name[4];
    int age;
} Student;

Student *p;
p = (Student*)mmap(NULL,sizeof(Student)*5,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
if(p == MAP_FAILED)
    handle_error("mmap");
    
/* 移动p指针回到起始位置(否则munmap报错"munmap: Invalid argument") */
/* p -= (N-1)*copy_block; */
/* 解除映射 */
if((munmap(p,sizeof(Student)*5)) == -1)
    handle_error("munmap");
```
5. 解除共享内存区的映射
```
if((munmap(p,sizeof(Student)*5)) == -1)
    handle_error("munmap");
```
6. 操作共享内存区(模仿对文件的操作就可以了)
```
Student *p;
p = (Student*)mmap(NULL,sizeof(Student)*5,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
if(p == MAP_FAILED)
    handle_error("mmap");

char ch = 'A';
for(int i=0;i<5;++i)
{
    memcpy((p+i)->name,&ch,1);
    (p+i)->age = 20+i;
    ch++;
}

if((munmap(p,sizeof(Student)*5)) == -1)
    handle_error("munmap");
```
7. [基于共享内存区读写学生结构体](https://github.com/orris27/orris/tree/master/process/ipc/codes/shared-rw)


8. 创建1个key=1234,大小为学生结构体的共享内存
```
int shmid = shmget(1234,sizeof(Student),0666|IPC_CREAT);
if(shmid == -1)
    handle_error("shmget");
```
9. 打开key=1234的共享内存
```
int shmid = shmget(1234,0,0);
if(shmid == -1)
    handle_error("shmget");
```
10. 连接共享内存
    + 默认使用`char*`就好
```
//Student *p = (Student*)shmat(shmid,NULL,0);

char *p = (char*)shmat(shmid,NULL,0);
if(p == (void*)-1)
    handle_error("shmat");
```

11. 脱离共享内存
```
if((shmdt(p)) == -1)
    handle_error("shmdt");
```
12. 删除key为1234的共享内存
```
int shmid = shmget(1234,0,0);
if(shmid == -1)
      handle_error("shmget");
      
if((shmctl(shmid,IPC_RMID,NULL)) == -1)
      handle_error("msgctl");
```
13. [共享内存的读写](https://github.com/orris27/orris/tree/master/process/ipc/codes/shm-rw)


14. [父子进程间通信](https://github.com/orris27/orris/tree/master/process/ipc/codes/mmap-ipc)

#### 2-1-3. 信号量 
1. 创建1个信号量集:
    1. key=1234,信号量集个数=1,选项=创建|不允许创建2次|权限
    ```
    int semid = semget(1234,1,IPC_CREAT|IPC_EXCL|0666);
    if(semid == -1)
        handle_error("semget");
    ```
    2. key随机
    ```
    key_t key = ftok(".",'s');//第一个参数必须是真实存在的路径,而第二个参数这里填's',其他的也可以
    int semid = semget(key,1,IPC_CREAT|IPC_EXCL|0666);
    if(semid == -1)
        handle_error("semget");
    ```
    3. 只由父子进程共享
    ```
    int semid = semget(IPC_PRIVATE,1,IPC_CREAT|IPC_EXCL|0666);
    if(semid == -1)
          handle_error("semget");
    ```
2. 打开key为1234的信号量集
```
int semid = semget(1234,0,0);
if(semid == -1)
    handle_error("semget");

```


3. 设置信号量集中第1个信号量的值
    + 如果要多个信号量同时设置初始值的话,注意添加循环
```
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf; 
};

void sem_setval(int semid,int val)
{
    
    union semun su; // 定义第4个参数
    su.val = val; // 赋值
    if((semctl(semid,0,SETVAL,su)) == -1) // 设置信号量集中第一个信号量的计数值+错误处理
        handle_error("semctl");
}

// ...

setval(semid,5);
```

4, 获取信号量集中信号量的值
```
int sem_getval(int semid)
{
    int ret = semctl(semid,0,GETVAL,0);
    if (ret == -1)
        handle_error("semctl");
    return ret;
}

// ....

printf("val=%d\n",getval(semid));

```

5. 删除信号量集
```
void sem_delete(int semid)
{
    
    if((semctl(semid,0,IPC_RMID,0)) == -1) // 删除信号量集:semid,0,IPC_RID
        handle_error("semctl");
    
}
rm_sem(semid);
```


6. 创建IPC对象的key值
```
key_t key = ftok(".",'s');//第一个参数必须是真实存在的路径,而第二个参数这里填's',其他的也可以
//semget(key,xxxx);
```


7. 执行P操作
    1. 对信号量集的第1个信号量进行P操作
    ```
    void sem_p(int semid)
    {
        struct sembuf sops[1] = {
            {0,-1,0}
        };
        if ((semop(semid,sops,1)) == -1)
            handle_error("semop");
    }
    //...
    sem_p(semid);
    ```
    2. 对信号量集的连续2个信号量进行P操作
        + 同时对2个信号量进行P操作的话,这2个信号量集是原子的
        + sem1和sem2是第sem1个和sem2个信号量,编号从0开始
    ```
    struct sembuf sops[2] = {
        {sem1,-1,0},
        {sem2,-1,0}
    };
    if ((semop(semid,sops,2)) == -1)
        handle_error("semop");
    ```

8. 执行V操作
    1. 对信号量集的第1个信号量进行V操作
    ```
    void sem_v(int semid)
    {
        struct sembuf sops[1] = {
            {0,+1,0}
        };
        if ((semop(semid,sops,1)) == -1)
            handle_error("semop");
    }
    //....
    sem_v(semid);
    ```
    2. 对信号量集的连续2个信号量进行V操作
        + 同时对2个信号量进行V操作的话,这2个信号量集是原子的
        + sem1和sem2是第sem1个和sem2个信号量,编号从0开始    
    ```
    struct sembuf sops[2] = {
        {sem1,+1,0},
        {sem2,+1,0}
    };
    
    if ((semop(semid,sops,2)) == -1)
        handle_error("semop");
    ```

9. [信号量集的基本使用](https://github.com/orris27/orris/tree/master/process/ipc/codes/semtool)


10. 打印1个字符并且不回车
    + 如果printf没有回车,而且没有刷新缓冲区的话,就不会立刻输出出去.所以一定要fflush
```
printf("%c",'a');
fflush(stdout); 
sleep(5);
```

11. 生成随机数: `rand()`默认生成0~INT_MAX的数字
```
srand(getpid());
int a = rand() % 3;
```

12. [基于信号量集实现偶数次字符打印](https://github.com/orris27/orris/tree/master/process/ipc/codes/print)
13. [哲学家就餐问题](https://github.com/orris27/orris/tree/master/process/ipc/codes/dinner)
14. [生产者和消费者的shmfifo解决](https://github.com/orris27/orris/tree/master/process/ipc/codes/produce-consume)
15. [单个信号量的信号量集的头文件和函数](https://github.com/orris27/orris/tree/master/process/ipc/codes/produce-consume),使用里面的`sem.c`和`sem.h`就行了


### 2-2. POSIX
#### 2-2-1. 消息队列
1. 查看创建的POSIX消息队列状态
```
mkdir /dev/mqueue
mount -t mqueue none /dev/mqueue
cd /dev/mqueue/
cat abc # 这里abc是创建的时候的第一个参数"/abc"对应的消息队列
```
2. 创建名字为"/abc"的POSIX消息队列(连接时需要加入`-lrt`参数,使用POSIX都需要,以下不赘述这个细节)
```
mqd_t mqid = mq_open("/abc",O_RDWR|O_CREAT,0666,NULL);
if(mqid == (mqd_t) -1)
    handle_error("mq_open");

if((mq_close(mqid)) == -1)
    handle_error("mq_close");
```
3. 打开名字为"/abc"的POSIX消息队列
```
mqd_t mqid = mq_open("/abc",O_RDONLY);
if(mqid == (mqd_t) -1)
    handle_error("mq_open");

if((mq_close(mqid)) == -1)
    handle_error("mq_close");
```
4. 关闭POSIX消息队列(一般打开/创建后都需要关闭消息队列)
```
if((mq_close(mqid)) == -1)
    handle_error("mq_close");
``` 
5. 删除名字为"/abc"的POSIX消息队列
```
mqd_t mqid = mq_open("/abc",O_RDONLY);
if(mqid == (mqd_t) -1)
      handle_error("mq_ope");

if((mq_unlink("/abc")) == -1)
    handle_error("mq_unlink");
```
6. 获取POSIX消息队列的状态
```
mqd_t mqid = mq_open("/abc",O_RDONLY);
if(mqid == (mqd_t) -1)
      handle_error("mq_ope");

struct mq_attr attr;
if((mq_getattr(mqid,&attr)) == -1)
    handle_error("mq_getattr");

printf("maxmsg=%ld\n",attr.mq_maxmsg);
printf("msgsize=%ld\n",attr.mq_msgsize);
printf("curmsgs=%ld\n",attr.mq_curmsgs);

if((mq_close(mqid)) == -1)
      handle_error("mq_close");
```
7. 发送消息给POSIX消息队列
    1. 参数
        + orris:Student结构体的一个普通变量
        + atoi(argv[1]):命令行参数,比如说0,1,2都可以
    2. 注意:需要O_RDWR方式打开
```
if((mq_send(mqid,(char*)&orris,sizeof(orris),atoi(argv[1]))) == -1)
    handle_error("mq_send");
```
8. 接收POSIX消息
```
struct mq_attr attr;
if((mq_getattr(mqid,&attr)) == -1)
    handle_error("mq_getattr");

Student stu;
unsigned int prio;
if((mq_receive(mqid,(char*)&stu,attr.mq_msgsize,&prio)) == -1)
    handle_error("mq_send");

printf("prio=%u name=%s age=%d\n",prio,stu.name,stu.age);

```
9. 注册信号处理函数的POSIX通知事件
```
mqd_t mqid;
struct sigevent sigev;

void handle_sigusr1(int sig)
{
    if((mq_notify(mqid,&sigev)) == -1) // 再次注册通知事件
        handle_error("mq_notify");

    struct mq_attr attr; // 接收消息
    if((mq_getattr(mqid,&attr)) == -1)
        handle_error("mq_getattr");

    Student stu;
    unsigned int prio;
    if((mq_receive(mqid,(char*)&stu,attr.mq_msgsize,&prio)) == -1)
        handle_error("mq_send");

    printf("prio=%u name=%s age=%d\n",prio,stu.name,stu.age); // 打印接收到的消息
}


int main()
{


    //...

    if((signal(SIGUSR1,handle_sigusr1)) == SIG_ERR)// 下面5行是为了注册通知事件并通过SIGUSR1信号处理
        handle_error("signal");
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    if((mq_notify(mqid,&sigev)) == -1)
        handle_error("mq_notify");
    //...

}

```

10. 等待信号,使进程处于等待信号的状态
```
while(1)
    pause();
```
11. [POSIX的消息队列实现](https://github.com/orris27/orris/tree/master/process/ipc/codes/posix-mq)

#### 2-2-2. 共享内存
1. 查看创建的POSIX共享内存才能状态
```
cd /dev/shm/
cat abc # 这里abc是创建的时候的第一个参数"/abc"对应的消息队列
```
1. 创建1个POSIX共享内存
    1. 简洁版本
    ```
    int shmfd = shm_open("/abc",O_RDWR|O_CREAT,0666);
    if(shmfd == -1)
        handle_error("shm_open"); 
    
    close(shmfd);
    ```
    2. 完整版本:添加修改大小
    ```
    int shmfd = shm_open("/abc",O_RDWR|O_CREAT,0666);
    if(shmfd == -1)
        handle_error("shm_open"); 
    if((ftruncate(shmfd,sizeof(Student))) == -1)
        handle_error("ftruncate");

    close(shmfd);
    ```
2. 打开1个POSIX共享内存
```
int shmfd = shm_open("/abc",O_RDWR,0);
if(shmfd == -1)
    handle_error("shm_open"); 
    
close(shmfd);
```
3. 删除POSIX共享内存
```
int shmfd = shm_open("/abc",O_RDWR,0);
if(shmfd == -1)
    handle_error("shm_open"); 

if((shm_unlink("/abc")) == -1)
    handle_error("shm_unlink");
```
4. 获取POSIX共享属性,并打印
```
struct stat statbuf;
if((fstat(shmfd,&statbuf)) == -1)
    handle_error("fstat");

printf("size=%ld\n",statbuf.st_size); // 打印共享内存的大小

printf("mode=%o\n",statbuf.st_mode & 07777); // 打印共享内存的权限
```

5. 映射POSIX共享内存,并写入数据
    1. 简洁版
    ```
    Student *p;
    p = (Student*)mmap(NULL,sizeof(Student),PROT_READ|PROT_WRITE,MAP_SHARED,shmfd,0);
    if(p == NULL)
          handle_error("mmap");
    ```
    2. 完整版
    ```
    typedef struct {
        char name[32];
        int age;
    } Student;


    Student *p;
    p = (Student*)mmap(NULL,sizeof(Student),PROT_READ|PROT_WRITE,MAP_SHARED,shmfd,0);
    if(p == NULL)
          handle_error("mmap");

    Student orris; // 构造存储的变量
    strcpy(orris.name,"orris");
    orris.age = 15;

    memcpy(p,&orris,sizeof(Student)); // 写入变量到p地址
    ```
6. [基于POSIX的共享内存实现的简单学生结构体的读写](https://github.com/orris27/orris/tree/master/process/ipc/codes/posix-shm)
#### 2-2-3. 线程
1. 创建1个新线程
    1. 不传递参数
    ```
    void* handle_thread(void *arg)
    {   

    }


    int main()
    // ...

    pthread_t tid; // 定义保存线程id的变量

    int ret;
    if((ret = pthread_create(&tid,NULL,handle_thread,NULL)) != 0 ) // 创建1个线程:变量,默认的属性,入口地址,参数+错误处理(从返回值获取错误代码,错误退出)
    {
        fprintf(stderr,"pthread_create:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    //....
    ```
    2. 传递1个int参数(conn_sockfd是int类型)
    ```
    
    pthread_t tid;
    int ret;
    int *p = (int*)malloc(sizeof(int)); // 构造传递的参数
    *p = conn_sockfd;

    if((ret = pthread_create(&tid,NULL,handle_thread,p)) != 0 )
    {
        fprintf(stderr,"pthread_create:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    
    // ...
    
    void* handle_thread(void *arg)

    //...
    int conn_sockfd = *(int*)arg;
    free(arg); // 释放参数

    ```
    
    3. 属性是分离属性的新线程
    ```
    
    int ret; // 定义保存pthread的返回值的变量
    
    pthread_t tid; // 定义保存线程id的变量
    
    pthread_attr_t attr; // 定义线程属性的变量
    
    if((ret = pthread_attr_init(&attr)) != 0 ) // 初始化线程属性的变量
    {
        fprintf(stderr,"pthread_attr_init:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    
    int detachstate = PTHREAD_CREATE_DETACHED; // 定义保存线程的分离属性的变量
    
    if((ret = pthread_attr_setdetachstate(&attr,detachstate)) != 0 ) // 设置线程的分离属性
    {
        fprintf(stderr,"pthread_attr_setdetachstate:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    
    if((ret = pthread_create(&tid,&attr,handle_thread,NULL)) != 0 ) // 创建1个线程:变量,使用新的分离属性,入口地址,参数+错误处理(从返回值获取错误代码,错误退出)
    {
        fprintf(stderr,"pthread_create:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    
    if((ret = pthread_attr_destroy(&attr)) != 0 ) // 销毁线程属性的变量
    {
        fprintf(stderr,"pthread_attr_destroy:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    ```
2. 等待新线程执行完毕,并回收尸体
    1. 参数
        + ret:int类型
        + tid:等待的线程id
    2. 等待新线程执行完毕,并回收尸体,不使用其返回值
    + 如果是多个线程的话,tid放到线程的数组里面,然后遍历数组,对每个元素调用pthread_join就可以了
    ```
    if((ret = pthread_join(tid,NULL)) != 0 )
    {
        fprintf(stderr,"pthread_join:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    ```
    3. 等待新线程执行完毕,并回收尸体,使用其返回值(比如说线程退出时输出"abc")
    ```
    void *retval;
    
    if((ret = pthread_join(tid,&retval)) != 0 ) // 等待新线程
    {
        fprintf(stderr,"pthread_join:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    printf("\nretval=%s\n",(char*)retval); 

    ```
3. 睡20微秒(可以用在线程的for循环中,从而可以观察线程的切换)
```
usleep(20);
```
4. 退出线程
    1. 退出线程,返回值为NULL
    ```
    pthread_exit(NULL);
    
    //return NULL;
    ```
    2. 退出线程,并带有返回值信息
    ```
    pthread_exit("pthread_exit");
    
    //return "ok";
    ```
5. 取消一个执行中的线程(类似于进程的kill)
    1. 注意:如果主线程中接收了线程的退出值,那么有可能会出问题,因为杀死线程后被杀死的线程不会有退出值
```
if((ret = pthread_cancel(tid)) != 0 )
{
    fprintf(stderr,"pthread_cancel:%s\n",strerror(ret));
    exit(EXIT_FAILURE);
}

```
6. 脱离线程=>线程结束后即使主线程不调用join也能回收尸体而不变成僵尸线程
```
void* handle_thread(void *arg)
// ...在线程入口函数的第一个地方就使用

int ret;
if((ret = pthread_detach(pthread_self())) != 0 )
{
    fprintf(stderr,"pthread_detach:%s\n",strerror(ret));
    exit(EXIT_FAILURE);
}
```

7. 创建1个TSD的<key,value>
```


void destructor(void *value)
{
    /* 释放掉value值(如果value值是malloc出来的话) */
    free(value);
}


pthread_key_t key; 

/*
 * 下面的代码在main函数内
 */
 
/* 定义key的变量 */
pthread_key_create(&key,destructor);

```
8. 设置学生结构体作为TSD的value值
```
typedef struct {
    char name[32];
    int age;
}Student;



/* 构造value值 */
Student *orris = (Student*)malloc(sizeof(Student));
strcpy(orris->name,"orris");
orris->age = pthread_self()%11;
/* 设置TSD的key的值 */
pthread_setspecific(key,orris);
```


9. 获取TSD的value值
```
/* orris是Student*变量 */
memcpy(orris,pthread_getspecific(key),sizeof(Student));
```

10. 删除TSD的<key,value>
```
/* 删除key */
pthread_key_delete(key);
```

11. 线程内创建TSD的<key,value>(线程内创建key的话,必须保证只有1个线程去创建,其他的不能去创建)
```
pthread_once_t once_control = PTHREAD_ONCE_INIT;
void handle_once()
{
    /* 创建1个key */
    pthread_key_create(&key,destructor);
    /* 打印说正在创建key */
    printf("Creating key....\n");
}

/*
 * 线程的入口地址void* handle_thread(void *arg)
 */
void* handle_thread(void *arg)
{

/* 创建key.(线程内创建key的话,必须保证只有1个线程去创建,其他的不能去创建) */
pthread_once(&once_control,handle_once);


}
```
12. [POSIX线程的相关函数wait,attr,exit,key的使用](https://github.com/orris27/orris/tree/master/process/ipc/codes/thread)

13. 打印线程信息
```
printf("threadid=0x%x is setting key\n",(int)pthread_self());
```


#### 2-2-4. 信号量
1. 创建POSIX信号量,并初始化POSIX信号量为无名信号量:初始值=10
```
sem_t sem_produce;
if((sem_init(&sem_produce,0,10)) == -1)
    handle_error("sem_init");



if((sem_destroy(&sem_produce)) == -1)
    handle_error("sem_destroy");

```
2. 执行POSIX信号量的P操作
```
if((sem_wait(&sem_produce)) == -1)
    handle_error("sem_wait");
```
3. 执行POSIX信号量的V操作
```
if((sem_post(&sem_produce)) == -1)
    handle_error("sem_post");
```
4. 销毁POSIX信号量
```
if((sem_destroy(&sem_produce)) == -1)
    handle_error("sem_destroy");
```
#### 2-2-5. 互斥锁
1. 创建POSIX互斥锁,为无名互斥锁:属性=NULL
```
pthread_mutex_t mutex;
pthread_mutex_init(&mutex,NULL);



pthread_mutex_destroy(&mutex);
```
2. 锁定
```
/* 锁定 */
pthread_mutex_lock(&mutex);
```
3. 解锁POSIX互斥锁
```
pthread_mutex_unlock(&mutex);
```
4. 销毁POSIX互斥锁
```
pthread_mutex_destroy(&mutex);
```
5. [生产者和消费者模型,基于POSIX信号量和互斥锁实现](https://github.com/orris27/orris/tree/master/process/ipc/codes/produce-consume)


#### 2-2-6. 条件变量
先占用互斥区,但是如果发现条件不满足就释放互斥区睡觉了,如果有人叫醒我,我就看看条件有没有满足,如果没有就继续睡觉,否则就开始占用互斥区工作了
1. 消费者等待条件变量
    1. 没有超时时间
    ```
    pthread_cond_t cond;
    
    pthread_mutex_lock(&mutex);
    /* while(仓库里没有产品) */
    while(total == 0)
    {
        /* 等待仓库里有产品的条件 */
        pthread_cond_wait(&cond,&mutex);
    }
    /* 消费产品:直接自减仓库产品数量 */
    total--;
    /* 解锁"占用位置"的资源 */
    pthread_mutex_unlock(&mutex);
    ```
    2. 有超时时间
    ```
    struct timespec abstime;
    /* 获取当前时间到abstime */
    clock_gettime(CLOCK_REALTIME,&abstime);
    /* 设置超时时间为2s */
    abstime.tv_sec = 2;

    pthread_mutex_lock(&mutex);
    while(total == 0)
    {
        int ret = pthread_cond_timedwait(&cond,&mutex,abstime);
        /* 如果是超时了 */
        if(ret == ETIMEDOUT)
        {
            /* 超时处理*/
        }
    }
    total--;
    pthread_mutex_unlock(&mutex);
    
    ```

2. 生产者发送信号通知(忙碌状态的线程不会接收到通知)
    1. 发起一个通知给第一个等待该条件变量的线程
    ```
    /* 锁定"占用位置"的资源 */
    pthread_mutex_lock(&mutex);
    /* 生产产品:仓库内的产品数量+1 */
    total++;
    /* 通知条件变量 */
    pthread_cond_signal(&cond);
    /* 解锁"占用位置"的资源 */
    pthread_mutex_unlock(&mutex);
    ```
    2. 发起广播给所有等待条件变量的线程
    ```
    pthread_mutex_lock(&mutex);
    
    total++;
    //condition_broadcast(&pool->ready);
    pthread_cond_broadcast(&cond);
    
    pthread_mutex_unlock(&mutex);
    ```

3. [生产者和消费者模型,基于POSIX条件变量和互斥锁实现](https://github.com/orris27/orris/tree/master/process/ipc/codes/produce-consume)


4. [基于POSIX的条件变量和线程实现线程池](https://github.com/orris27/orris/tree/master/process/ipc/codes/threadpool)
    1. 条件变量和互斥锁共同配合的头文件接口实现
    2. 线程池等待所有(忙碌+等待)线程都结束后执行销毁



#### 2-2-7. 文件锁(进程间可以使用,线程间不可以使用)
1. 锁定整个文件
```
/* 构造文件锁的结构变量 */
struct flock f_lock;
//f_lock.l_type = F_RDLCK; //读锁
f_lock.l_type = F_WRLCK; // 写锁
f_lock.l_whence = SEEK_SET;
f_lock.l_start = 0;
f_lock.l_len = 0;
/* 锁定该文件:写 */
fcntl(fd,F_SETLKW,&f_lock);
```
2. 解锁整个文件
```
/* 构造文件锁的结构变量 */
struct flock f_lock;
f_lock.l_type = F_UNLCK;
f_lock.l_whence = SEEK_SET;
f_lock.l_start = 0;
f_lock.l_len = 0;
/* 锁定该文件:写 */
fcntl(fd,F_SETLKW,&f_lock);
```


### 2-3. 管道
1. 创建1个管道
```
int fds[2]; // 定义保存读端/写端的2个套接字的数组
if((pipe(fds)) == -1) // 创建1个管道
    handle_error("pipe");
```

2. 创建1个管道,父进程写,子进程读
```
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



    /* read(fds[0],xx);*/


}
else // 父进程
{
    /* 关闭读端套接字(第1个套接字) */
    close(fds[0]);
    
    
    
    /* write(fds[1],xxx) */



}

```



## 3. UNIX编程
1. [制作静态库和制作动态库](https://github.com/orris27/orris/blob/master/cpp/cpp.md)

2. 重定向stdout到指定的文件描述符
```
if((dup2(fd,STDOUT_FILENO)) == -1)
    handle_error("dup2");
```


3. 删除当前目录下的test文件
```
if((unlink("test")) == -1)
    handle_error("unlink");
```

4. 创建临时文件:创建文件后立刻减少硬链接数
```
/* 创建文件 */
int fd;
if((fd = open("tmpfile",O_RDWR|O_CREAT,0666)) == -1)
    handle_error("open");
/* 减少文件的硬链接数 */
if((unlink("tmpfile")) == -1)
    handle_error("unlink");


/* 处理文件 (这里写对临时文件的处理内容~~)
if((write(fd,"hello\n",6)) == -1)
    handle_error("write");

lseek(fd,0,SEEK_SET);
char buf[1024];
memset(buf,0,sizeof(buf));
read(fd,buf,sizeof(buf));
fputs(buf,stdout);
sleep(10);
*/


/* 关闭文件 */
close(fd);
```
5. 修改进程所在目录
```
/* 修改进程所在的目录在上一级别 */
if((chdir("../")) == -1)
    handle_error("chdir");

```
6. 打开目录
```
/* 构造目录的结构变量 */
DIR *p_dir;
/* 打开目录+错误处理 */
p_dir = opendir(dirname);
if(p_dir == NULL)
    handle_error("opendir");
/* 构造目录内容的结构变量 */
struct dirent *p_dirent;
/* 遍历当前的目录 */
/* while(读取目录,并且是有内容可以读的:返回值不为NULL) */
while((p_dirent = readdir(p_dir)) != NULL)
{


    /* 对目录下面包括.和..在内的文件进行处理 */

    /* 文件名:(字符串)p_dirent->d_name */
    /* 查看文件类型:p_dirent->d_type == DT_DIR或者DT_REG等*/




}
/* 关闭目录 */
closedir(p_dir);
```
7. 打开目录并遍历里面的内容,最终获得普通文件总数
```
int count_regs(char *dirname)
{
    /* 定义普通文件总数 */
    int count = 0;
    /* 构造目录的结构变量 */
    DIR *p_dir;
    /* 打开目录+错误处理 */
    p_dir = opendir(dirname);
    if(p_dir == NULL)
        handle_error("opendir");
    // 构造目录内容的结构变量
    struct dirent *p_dirent;
    // 遍历当前的目录
    // while(读取目录,并且是有内容可以读的:返回值不为NULL)
    while((p_dirent = readdir(p_dir)) != NULL)
    {
        // 如果是.和..
        if(strcmp(p_dirent->d_name,".") == 0||strcmp(p_dirent->d_name,"..") == 0)
            // 跳过
            continue;
        // 如果是目录
        else if(p_dirent->d_type == DT_DIR)
        {
            // 构造新目录的绝对目录
            char new_dirname[1024];
            memset(new_dirname,0,sizeof(new_dirname));
            sprintf(new_dirname,"%s/%s",dirname,p_dirent->d_name);

            // 读取新目录+更新普通文件总数
            count += count_regs(new_dirname);
        }
        // 如果是普通文件
        else if(p_dirent->d_type == DT_REG)
            // 增加文件总数
            count++;
    }
    // 关闭目录
    closedir(p_dir);
    // 返回普通文件总数
    return count;
    
}

```
8. 拼接2个字符串到一个新的字符串
```
sprintf(new_dirname,"%s/%s",dirname,p_dirent->d_name);
```
9. 打印出所有的环境变量
```
/* 获取环境变量的数组 */
extern char **environ;
int main()
{
//...
    
    /* 循环遍历environ数组,一直到数组元素的值为NULL的时候停止 */
    for(int i=0;environ[i];++i)
    {
        /* 打印输出环境变量  */
        printf("%s\n",environ[i]);
    }

//...
}
```
10. 获得PATH的环境变量
```
char* buf;
buf = getenv("PATH");

printf("PATH=%s\n",buf);
```
11. 在C语言中的子进程内执行命令行中的ls命令
```
// 创建1个进程
pid_t pid;
if ((pid = fork()) == -1)
    handle_error("fork");
if (pid == 0) // 子进程
{
    /* 执行ls命令(使用execlp函数):文件名,argv[0],argv[1],NULL */
    /* 使用execlp引入PATH变量
    if((execlp("ls","ls","-l","-a",NULL)) == -1)
        handle_error("execl");
    */ 
    if((execl("/bin/ls","/bin/ls","-l","-a",NULL)) == -1)
        handle_error("execl");
}
else // 父进程
{
}


```
12. 命令行错误提示
```
if(argc != 3)
{
    fprintf(stderr,"Usage:%s <src> <dest>\n",argv[0]);
    exit(EXIT_FAILURE);
}

```

13. 获取文件大小
```
/* 获取源文件的大小size */
struct stat statbuf;
if((fstat(fd,&statbuf)) == -1)
      handle_error("fstat");
int size = (int)statbuf.st_size;

```

14. [多进程拷贝实现](https://github.com/orris27/orris/tree/master/process/ipc/codes/multicopy)
15. 屏蔽SIGQUIT信号.添加SIGQUIT信号到阻塞信号集(信号屏蔽字)
```
/* 定义我们的信号集变量 */
sigset_t my_set;
/* 清洗信号集变量 */
if((sigemptyset(&my_set)) == -1)
    handle_error("sigemptyset");
/* 添加SIGQUIT信号到我们的信号集 */
if((sigaddset(&my_set,SIGQUIT)) == -1)
    handle_error("sigaddset");
/* 修改屏蔽信号集 */
if((sigprocmask(SIG_BLOCK,&my_set,NULL)) == -1)
    handle_error("sigprocmask");

```



16. 打印未决信号集
```
/* 获取未决信号集 */
if((sigpending(&pending_set)) == -1)
    handle_error("sigpending");
for(int i=1;i<32;++i)
{
    /* 如果当前信号在未决信号集里 */
    if(sigismember(&pending_set,i))
    {
        printf("1");
    }
    /* 如果当前信号不在未决信号集里 */
    else
    {
        printf("0");
    }
}
printf("\n");
/* 刷新缓冲区 */
fflush(stdout);
```
17. 使用sigaction注册信号捕捉函数,并且在处理期间屏蔽SIGQUIT,并且使用默认设置(捕捉函数处理期间屏蔽本信号)
```
/* 构造sigaction的结构体变量 */
struct sigaction act;
/* 设置捕捉函数 */
act.sa_handler = handle_sigint;
/* 清洗信号集变量 */
if((sigemptyset(&act.sa_mask)) == -1)
      handle_error("sigemptyset");
/* 添加SIGQUIT信号到我们的信号集 */
if((sigaddset(&act.sa_mask,SIGQUIT)) == -1)
      handle_error("sigaddset");
/* 设置捕捉函数处理期间是否屏蔽本信号 */
act.sa_flags = 0;
/* 注册SIGINT的的捕捉函数+错误处理 */
if((sigaction(SIGINT,&act,NULL)) == -1)
    handle_error("sigaction");

```
18. [自定义sleep解决时序竞态问题实现](https://github.com/orris27/orris/tree/master/cpp/codes/my_sleep)
```
/*
 * SIGALRM处理函数
 */
void handle_sigalrm(int sig)
{
    // 什么都不做
}

/*
 * my_sleep函数
 */
unsigned int my_sleep(unsigned int seconds)
{
    // 定义保存SIGALRM的旧处理函数的变量
    struct sigaction oldact;
    // 注册SIGALRM处理函数,并取出旧的SIGALRM处理函数
    /* 构造sigaction的结构体变量 */
    struct sigaction act;
    /* 设置捕捉函数 */
    act.sa_handler = handle_sigalrm;
    /* 清洗信号集变量 */
    if((sigemptyset(&act.sa_mask)) == -1)
            handle_error("sigemptyset");
    /* 设置捕捉函数处理期间是否屏蔽本信号 */
    act.sa_flags = 0;
    /* 注册SIGINT的的捕捉函数+错误处理 */
    if((sigaction(SIGALRM,&act,&oldact)) == -1)
          handle_error("sigaction");


    // 定义保存旧的阻塞信号集的变量
    sigset_t oldset;

    /* 定义我们的信号集变量 */
    sigset_t my_set;
    /* 清洗信号集变量 */
    if((sigemptyset(&my_set)) == -1)
          handle_error("sigemptyset");
    /* 添加SIGQUIT信号到我们的信号集 */
    if((sigaddset(&my_set,SIGALRM)) == -1)
          handle_error("sigaddset");
    /* 修改屏蔽信号集 */
    if((sigprocmask(SIG_BLOCK,&my_set,&oldset)) == -1)
          handle_error("sigprocmask");

    // 保存旧的阻塞信号集到挂起期间的阻塞信号集
    sigset_t pause_set;
    memcpy(&pause_set,&oldset,sizeof(sigset_t));
    // 删除挂起期间的阻塞信号集中的SIGALRM信号
    if((sigdelset(&pause_set,SIGALRM)) == -1)
          handle_error("sigdelset");
    
    // 开始计时
    alarm(seconds);
    // 解除SIGALRM信号屏蔽(使用挂起期间的阻塞信号集),并且挂起
    sigsuspend(&pause_set);

    // 停止计时,并获得还剩多少时间(防止如果挂起期间被其他信号打断,这时主动关闭计时器)
    unsigned int ret = alarm(0);
    // 恢复SIGALRM的旧处理函数
    if((signal(SIGALRM,oldact.sa_handler)) == SIG_ERR)
          handle_error("signal");
    // 恢复旧的阻塞信号集
    if((sigprocmask(SIG_SETMASK,&oldset,NULL)) == -1)
          handle_error("sigprocmask");
    
    // 返回本来还剩多少时间发出闹钟信号
    return ret;
}


```
19. [父子进程交替数数导致的全局变量异步I/O问题](https://github.com/orris27/orris/tree/master/cpp/codes/async-count)
20. 打印stdin,stdout和stderr的终端名字
```

/* 打印stdin,stdout和stderr的设备名称 */
for(int i=0;i<3;++i)
    printf("fd %d:%s\n",i,ttyname(i));

#----------------------------------------
# fd 0:/dev/pts/0
# fd 1:/dev/pts/0
# fd 2:/dev/pts/0
#----------------------------------------


```
21. 打印进程id,打印进程组id,打印会话id`
```
/* 打印pid */
printf("pid:%d\n",getpid());
/* 打印gid */
printf("gid:%d\n",getpgid(0));
/* 打印sid */
if((printf("sid:%d\n",getsid(0))) == (pid_t) -1)
    handle_error("getsid");
```


22. 创建1个新会话
```
/* 创建1个子进程 */
pid_t pid; 
if ((pid = fork()) == -1)
      handle_error("fork");
      
/* 如果是父进程 */
if (pid > 0)
{
    /* 直接退出 */
    return 0;
}

/* 如果是子进程 */
/* 睡一会儿,等待父进程退出 */
/*sleep(1); */
/* 创建新的会话 */
if((setsid()) == (pid_t) -1)
    handle_error("setsid");

/*
 * 自由发挥~~~
 */


```
23. 创建1个守护进程,在`/home/orris/tmp/daemon`下创建20个文件
```
/* 创建1个子进程 */
pid_t pid; 
if ((pid = fork()) == -1)
        handle_error("fork");
      
/* 如果是父进程 */
if (pid > 0)
{
    /* 直接退出 */
    return 0;
}

/* 创建新的会话 */
if((setsid()) == (pid_t) -1)
      handle_error("setsid");

/* 设置工作目录为根目录或者其他目录 */
if((chdir("/home/orris/tmp/daemon")) == -1)
      handle_error("chdir");
/* 设置自己的umask */
umask(0022); // 0666创建出来的文件就是0644的权限
/* 重定向0,1,2文件描述符到黑洞或关闭文件描述符 */
/* 关闭stdin文件描述符 */
close(STDIN_FILENO);

/* 打开黑洞 */
int fd = open("/dev/null",O_RDWR,0);
if(fd == -1)
      handle_error("open");
/* 重定向stdout到0 */
if((dup2(0,STDOUT_FILENO)) == -1)
      handle_error("dup2");
/* 重定向stderr到0 */
if((dup2(0,STDERR_FILENO)) == -1)
      handle_error("dup2");
/* 守护进程主逻辑 */
handle_daemon();
/* 退出:0 */
return 0;

```

24. 循环创建不同的文件名(思路:字符变量的+1)
```
/* 创建文件名的缓冲区 */
char buf[32];
strcpy(buf,"apple");
for(int i=0;i<20;++i)
{
    /* 修改文件名 */
    buf[0] += 1;
    
    /*
     *  ~~~
     */ 
}


```
25. IO
    1. 设置文件描述符的IO为非阻塞
    ```
    int flag = fcntl(sockfd,F_GETFL);
    if(flag == -1)
        handle_error("fcntl");
    flag |= O_NONBLOCK;
    fcntl(sockfd,F_SETFL,flag);
    ```
    2. 非阻塞读
    ```
    while ((len = read(connfd, buf, MAXLINE/2)) >0 ) 
        write(STDOUT_FILENO, buf, len);
    ```
26. 清零结构体
```
bzero(&serv_addr,sizeof(serv_addr));
```


## 4. 普通C语言
1. scanf格式化输入
```
int pid;
int state;
char parent[100];
char name[100];

# input: "Nov 27 14:53:40 leoli1 kernel: [513289.047220] insmod 32226 0 bash"
scanf("%*[^\]]] %s %d %d %s", name, &pid, &state, parent);

printf("%s\n", name); # insmod
printf("%d\n", pid); # 32226
printf("%d\n", state); # 0
printf("%s\n", parent); # bash

```
2. 文件读写
```
FILE *fp;

fp = fopen("/var/log/kern.log", "r");
//文件的每一行基本为 "Nov 27 14:53:40 leoli1 kernel: [513289.047220] insmod 32226 0 bash"

while(!feof(fp))
{
    fscanf(fp, "%*[^\]]] %s %d %d %s", name, &pid, &state, parent);
    if(strcmp(name, "start") == 0)
        // ...
    // ...
}
fclose(fp);
```

## 5. AT&T
```
#include <stdio.h>

int main()
{
    int in = 10, out;
    __asm__("movl $4, %%eax\n\t" /* $4表示常数; 4表示内存地址 */
            "movl %%ebx, %0\n\t" 
            :"=r"(out)           
            :"b"(in)             /* input; 变量a的值放到寄存器%ebx(b表示寄存器%ebx)中 */
            :"%eax");           
    printf("in=%d out=%d\n", in, out);

    return 0;
}
```

## 6. cpp
1. header
```
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <deque>
#include <assert.h>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <stdio.h>
#include <string.h>
#include <utility>
#include <math.h>
#include <bitset>
#include <iomanip>
#include <complex>

using namespace std;

#define rep(i, a, b) for (int i = (a), i##_end_ = (b); i < i##_end_; ++i)
#define debug(...) fprintf(stderr, __VA_ARGS__)
#define mp make_pair
#define x first
#define y second
#define pb push_back
#define SZ(x) (int((x).size()))
#define ALL(x) (x).begin(), (x).end()

template<typename T> inline bool chkmin(T &a, const T &b) { return a > b ? a = b, 1 : 0; }
template<typename T> inline bool chkmax(T &a, const T &b) { return a < b ? a = b, 1 : 0; }
template<typename T> inline bool smin(T &a, const T &b)   { return a > b ? a = b : a;    }
template<typename T> inline bool smax(T &a, const T &b)   { return a < b ? a = b : a;    }

typedef long long LL;
```
2. sort
```
int cmp(int a, int b)
{
    return a < b; // increasing
    // return a > b; // decreasing or reversed = True
}
sort(o, o + n, cmp);
```
## 7. string
1. length
```
s = "abc";
s.length(); // 3
s.size(); // 3
```
2. substr
```
s = "abcdefg";
s.substr(1, 3); // "bcd"
s.substr(3, 2); // "de"
```
## 8. stack
```
stack<int> s;
cout << s.empty() << endl; # 1
s.push(10);
s.push(20);
cout << s.empty() << endl; # 0
cout << s.top() << endl; # 20
s.pop();
cout << s.top() << endl; # 10
s.pop();
cout << s.empty() << endl; # 1

```

## 9. vector
1. initialize vector
```
vector<int> vect;  
vect.push_back(10); 
vect.push_back(20); 
vect.push_back(30); 
  

vector<int> vect(3, 10);  // 10, 10, 10
vector<int> vect{ 10, 20, 30 };  // 10, 20, 30

vector<int> vect1{ 10, 20, 30 }; 
vector<int> vect2(vect1.begin(), vect1.end()); // 10, 20, 30

vector<vector<int>> intervals{{1, 3}, {2, 6}, {8, 10}, {15, 18}};
```

2. return
```
vector<int> get()
{
    return vector<int> {2, 3};
}
```
3. back: return the last element
```
vector<int> v{1, 2, 3, 4};
v.back(); // 4
```
## 10. map
1. insert
```
map<int, int> res;
res.insert(pair<int, int>(1, 100));
res.insert(pair<int, int>(2, 200));
```
2. find
```
res.find(1) == res.end() // true
res.find(2) == res.end() // true
res.find(3) == res.end() // false
```
3. second, iterator
```
map<int, int> m;
map<int, int>::iterator it;
m.insert(pair<int, int>(1, 10));
it = m.find(1);
cout << it->second << endl;

```
## 11. set
1. construct
```
set<char> chars;
```
2. insert
```
chars.insert('a');
```
3. find
```
if (chars.find('a') == chars.end())
    // ...
```
4. erase
```
chars.erase('a');
```

## 12. lambda
```
vector<vector<int>> v{{3, 2}, {1, 3}, {5, 8}};
sort(v.begin(), v.end(), [](const vector<int>& a, const vector<int>& b) {return a[0]<b[1];});
# v: {{1 , 3}, {3, 2}, {5, 8}}
```
## 13. iterate vector in C++
```
vector<vector<int>> v{{3, 2}, {1, 3}, {5, 8}};
//for (vector<int>& vi: v)  // same
for (auto& vi: v) // auto => vector<int> type
{
    for(int elm: vi) // int type
    {
        cout << elm << " ";
    }
    cout << endl;
}

```
