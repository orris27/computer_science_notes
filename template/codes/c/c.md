## 1. 网络编程
1. [回射C/S-TCP模型](https://github.com/orris27/orris/tree/master/network/socket/codes/simple-tcp).[图解](https://ask.qcloudimg.com/http-save/yehe-1147827/sams3iwj3k.png?imageView2/2/w/1620)
2. 创建ipv4地址
```
struct sockaddr_in addr;
memset(&addr,0,sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_port = htons(5188); 
addr.sin_addr.s_addr = inet_addr("127.0.0.1");

// (struct sockaddr*)&addr // 转换成通用地址结构体的指针,供connect等函数使用
```
3. 发送数据
    1. write(UNIX的流协议/TCP/socketpair下的UNIX流协议)
    ```
    char send_buf[1024];
    // fgets(send_buf,sizeof(send_buf),stdin);
    write(sockfd,send_buf,sizeof(send_buf)); //直接网套接字里写数据就行了
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
        ERR_EXIT("connect");
        
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
            ERR_EXIT("read");
    }
    ```
    2. UDP接收数据
    ```
    int ret = recvfrom(sockfd,recv_buf,sizeof(recv_buf),0,(struct sockaddr*)&peer_addr,&peer_len);
    if (ret == -1)// 如果read出现错误,就直接退出子进程
    {
        if (errno == EINTR)
            continue;   
        else //如果返回值<0,并且不是中断的信号=>退出进程,返回-1
            ERR_EXIT("read");
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
      ERR_EXIT("socket");
    ```
    2. UDP套接字
    ```
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)
      ERR_EXIT("socket");
    ```
    3. UNIX套接字
    ```
    int sockfd;
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0))<0)
        ERR_EXIT("socket");
    ```
    4. 创建UDP套接字并绑定ip地址
    ```
    int sockfd;
    struct sockaddr_in addr;
    
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
        ERR_EXIT("socket");
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0) 
        ERR_EXIT("bind");
    ```
    
    5. 创建TCP套接字并根据ip地址连接
    ```
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
      ERR_EXIT("socket");

    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0)
      ERR_EXIT("connect");
    ```
    6. 创建UNIX套接字,并删除原来的套接字文件,并绑定UNIX地址,并处于监听状态
    ```
    int sockfd;
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0))<0)
        ERR_EXIT("socket");

    struct sockaddr_un addr;
    memset(&addr,0,sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path,"/tmp/test-socket");

    unlink(addr.sun_path);
    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0)
        ERR_EXIT("bind");
    
    if(listen(sockfd,SOMAXCONN)<0)
        ERR_EXIT("listen");
    ```
    7. 创建UNIX套接字,并连接UNIX地址
    ```
    int sockfd;
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0))<0)
          ERR_EXIT("socket");

    struct sockaddr_un addr;
    memset(&addr,0,sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path,"test-socket");
    ```
    8. 创建套接字对(socketpair)
    ```
    int sockets[2];
    if(socketpair(AF_UNIX,SOCK_STREAM,0,sockets) == -1)
        ERR_EXIT("socketpair");

    ```
    9. 创建UNIX流协议的套接字对,并创建1个进程,父子进程各使用其中1个套接字
    ```
    int sockets[2];
    if(socketpair(AF_UNIX,SOCK_STREAM,0,sockets) == -1)
        ERR_EXIT("socketpair");
        
    pid_t pid;
    if ((pid = fork()) == -1)
        ERR_EXIT("fork");
    
    if (pid == 0) // (假定子进程使用第1个套接字,而父进程使用第2个套接字)
    {
        close(sockets[1]);
    }
    else
    {
        close(sockets[0]);
    }
    ```

11. 获取套接字地址
    1. 获取自身套接字地址
    ```
    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    if (getsockname(sockfd,(struct sockaddr*)&localaddr,&addrlen) < 0)
        ERR_EXIT("getsockname");
    ```
    2. 获取对方套接字地址(该套接字必须处于连接状态)
    ```
    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    if (getpeername(sockfd,(struct sockaddr*)&localaddr,&addrlen) < 0)
        ERR_EXIT("getpeername");
    ```

12. 使UNIX流协议/TCP套接字处于监听状态
```
if(listen(sockfd,SOMAXCONN)<0) // SOMAXCONN是服务器套接字允许建立的最大队列,包括未连接+已连接的队列
  ERR_EXIT("listen");
```

13. 允许服务器套接字重用TIME_WAIT套接字(实验中使用在bind函数之前,具体怎样不太清楚)
```
int reuse_on=1;
if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse_on,sizeof(reuse_on))<0)
  ERR_EXIT("setsockopt");
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
  ERR_EXIT("accept");
```
17. 获取主机名
```
char hostname[100]={0};
if (gethostname(hostname,sizeof(host) <0)
    ERR_EXIT("gethostname");
```

18. 获取本机的所有ip列表(通过主机名获取)(虚拟机能获取`10.0.0.7`,但是获取不了`127.0.0.1`,我自己的电脑只能获取到`127.0.1.1`)
```
// 获取主机名
char hostname[100]={0};
if(gethostname(hostname,sizeof(hostname)) < 0)
    ERR_EXIT("gethostname");


// 获取保存ip列表的结构体,通过主机名
struct hostent *hp;
if ((hp = gethostbyname(hostname)) == NULL)
    ERR_EXIT("gethostbyname");


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
    ERR_EXIT("getlocalip");
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
        signal(SIGCHLD,handle_sigchld);
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
    {
        signal(SIGCHLD,handle_sigchld);
        //....
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
      ERR_EXIT("getrlimit");
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
      ERR_EXIT("getrlimit");

//查看当前进程的文件描述符
if (getrlimit(RLIMIT_NOFILE,&rl) <0)
      ERR_EXIT("getrlimit");
printf("%d\n",(int)rl.rlim_cur);
printf("%d\n",(int)rl.rlim_max);
```


28. err_exit
```
#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)
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

30. Makefile
    1. 使用(下面的4个空格可能要换成tab键)
        1. 在BIN中写入要编译的c文件的文件名(不包括`.c`,比如说`client.c`就写成`BIN=client`)
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
        ERR_EXIT("fork");
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
              ERR_EXIT("fork");
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
            ERR_EXIT("sendmsg");

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
            ERR_EXIT("sendmsg");

        p_cmsg = CMSG_FIRSTHDR(&msg);
        if (p_cmsg == NULL)
            ERR_EXIT("no passed fd");


        p_fd = (int*)CMSG_DATA(p_cmsg);
        recvfd = *p_fd;
        if(recvfd == -1)
            ERR_EXIT("no passed fd");

        return recvfd;
    }
    ```




## 2. IPC
### 2-1. System V消息队列

1. 创建key为1234消息队列
```
int msgid = msgget(1234,0666|IPC_CREAT);
if(msgid == -1)
    ERR_EXIT("msgget");
```
2. 打开key为1234的消息队列
```
int msgid = msgget(1234,0);
if(msgid == -1)
    ERR_EXIT("msgget");
```
3. 删除消息队列
```
int msgid = msgget(1234,0);
if(msgid == -1)
    ERR_EXIT("msgget");
    
if((msgctl(msgid,IPC_RMID,NULL)) == -1)
    ERR_EXIT("msgctl");
```
4. 获得消息队列的状态,并输出它的信息
```
int msgid = msgget(1234,0);
if(msgid == -1)
      ERR_EXIT("msgget");

struct msqid_ds msg_stat;
if((msgctl(msgid,IPC_STAT,&msg_stat) == -1))
    ERR_EXIT("msgctl");

printf("mode=%o\n",msg_stat.msg_perm.mode);//输出消息队列的权限

printf("number=%d\n",(int)msg_stat.msg_qnum);//输出消息队列中消息的个数

printf("bytes=%ld\n",msg_stat.__msg_cbytes);//输出消息队列中所有消息的实际数据的字节数和

printf("msgmnb=%d\n",(int)msg_stat.msg_qbytes);//输出消息队列中所有消息的实际数据的字节数和的最大值
```
5. 设置消息队列的状态,比如说修改消息队列的权限
```
int msgid = msgget(1234,0);
if(msgid == -1)
      ERR_EXIT("msgget");

struct msqid_ds msg_stat;
if((msgctl(msgid,IPC_STAT,&msg_stat) == -1))
    ERR_EXIT("msgctl");

sscanf("600","%o",(unsigned int*)&msg_stat.msg_perm.mode);
if((msgctl(msgid,IPC_SET,&msg_stat) == -1))
    ERR_EXIT("msgctl");
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
      ERR_EXIT("msgsnd");
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
          ERR_EXIT("msgget");
    int bytes = atoi(argv[1]); // 发送的实际数据的字节数
    int type  = atoi(argv[2]); // 发送的消息类型
    
    struct msgbuf *p_buf = (struct msgbuf *)malloc(sizeof(long) + bytes);
    p_buf->mtype = type;
    if((msgsnd(msgid,p_buf,bytes,0)) == -1) //if((msgsnd(msgid,p_buf,bytes,IPC_NOWAIT)) == -1)
        ERR_EXIT("msgsnd");

    ```
7. 接收key为1234的消息队列的消息
    1. 参数
        1. mtype:接收哪个消息类型
        2. msgflg:接收消息的函数的使用选项
    2. 简洁用法
    ```
    if ((msgrcv(msgid,p_buf,MSGMAX,mtype,0)) == -1)
      ERR_EXIT("msgrcv");
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
          ERR_EXIT("msgget");
    struct msgbuf *p_buf = (struct msgbuf *)malloc(sizeof(long) + MSGMAX);

    if ((msgrcv(msgid,p_buf,MSGMAX,mtype,msgflg)) == -1)
        ERR_EXIT("msgrcv");

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
    if (opt == '?') // 出现新的参数.这里不使用ERR_EXIT是因为出现了新的参数也表示成功
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

### 2-2. System V共享内存
1. Linux下以读写方式打开/创建文件,并清空文件,设置权限为0666
    + open得到文件描述符,而fopen得到FILE指针
```
int fd = open(argv[1],O_CREAT|O_RDWR|O_TRUNC,0666)
if(fd == -1)
    ERR_EXIT("open");
```
2. 移动文件描述符
```
lseek(fd,sizeof(Student)*5-1,SEEK_SET); // 移动到Student结构体大小的5倍-1的位置
```
3. 填充5个Student结构体大小的空间,并设置为0
```
lseek(fd,sizeof(Student)*5-1,SEEK_SET);
write(fd,"",1);
```
4. 映射文件到共享内存区
```
typedef struct{
    char name[4];
    int age;
} Student;

Student *p;
p = (Student*)mmap(NULL,sizeof(Student)*5,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
```
5. 解除共享内存区的映射
```
if((munmap(p,sizeof(Student)*5)) == -1)
    ERR_EXIT("munmap");
```
6. 操作共享内存区(模仿对文件的操作就可以了)
```
Student *p;
p = (Student*)mmap(NULL,sizeof(Student)*5,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
if(p == NULL)
    ERR_EXIT("mmap");

char ch = 'A';
for(int i=0;i<5;++i)
{
    memcpy((p+i)->name,&ch,1);
    (p+i)->age = 20+i;
    ch++;
}

if((munmap(p,sizeof(Student)*5)) == -1)
    ERR_EXIT("munmap");
```
7. [基于共享内存区读写学生结构体](https://github.com/orris27/orris/tree/master/process/ipc/codes/shared-rw)


8. 创建1个key=1234,大小为学生结构体的共享内存
```
int shmid = shmget(1234,sizeof(Student),0666|IPC_CREAT);
if(shmid == -1)
      ERR_EXIT("shmget");
```
9. 打开key=1234的共享内存
```
int shmid = shmget(1234,0,0);
if(shmid == -1)
    ERR_EXIT("shmget");
```
10. 连接共享内存
```
Student *p = (Student*)shmat(shmid,NULL,0);
if(p == (void*)-1)
    ERR_EXIT("shmat");
```

11. 脱离共享内存
```
if((shmdt(p)) == -1)
    ERR_EXIT("shmdt");
```
12. 删除key为1234的共享内存
```
int shmid = shmget(1234,0,0);
if(shmid == -1)
      ERR_EXIT("shmget");
      
if((shmctl(shmid,IPC_RMID,NULL)) == -1)
      ERR_EXIT("msgctl");
```
13. [共享内存的读写](https://github.com/orris27/orris/tree/master/process/ipc/codes/shm-rw)
### 2-3. System V信号量
1. 创建1个信号量集:
    1. key=1234,信号量集个数=1,选项=创建|不允许创建2次|权限
    ```
    int semid = semget(1234,1,IPC_CREAT|IPC_EXCL|0666);
    if(semid == -1)
        ERR_EXIT("semget");
    ```
    2. key随机
    ```
    key_t key = ftok(".",'s');//第一个参数必须是真实存在的路径,而第二个参数这里填's',其他的也可以
    int semid = semget(key,1,IPC_CREAT|IPC_EXCL|0666);
    if(semid == -1)
        ERR_EXIT("semget");
    ```
    3. 只由父子进程共享
    ```
    int semid = semget(IPC_PRIVATE,1,IPC_CREAT|IPC_EXCL|0666);
    if(semid == -1)
          ERR_EXIT("semget");
    ```
2. 打开key为1234的信号量集
```
int semid = semget(1234,0,0);
if(semid == -1)
    ERR_EXIT("semget");

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
        ERR_EXIT("semctl");
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
        ERR_EXIT("semctl");
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
        ERR_EXIT("semctl");
    
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
            ERR_EXIT("semop");
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
        ERR_EXIT("semop");
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
            ERR_EXIT("semop");
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
        ERR_EXIT("semop");
    ```

9. [信号量集的基本使用](https://github.com/orris27/orris/tree/master/process/ipc/codes/semtool)


10. 打印1个字符并且不回车
    + 如果printf没有回车,而且没有刷新缓冲区的话,就不会立刻输出出去.所以一定要fflush
```
printf("%c",'a');
fflush(stdout); 
sleep(5);
```

11. 生成随机数
```
srand(getpid());
int a = rand() % 3;
```

12. [基于信号量集实现偶数次字符打印](https://github.com/orris27/orris/tree/master/process/ipc/codes/print)
13. [哲学家就餐问题](https://github.com/orris27/orris/tree/master/process/ipc/codes/dinner)
