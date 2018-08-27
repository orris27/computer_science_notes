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
```
// connnect(sockfd,....);
char send_buf[1024];
// fgets(send_buf,sizeof(send_buf),stdin);
write(sockfd,send_buf,sizeof(send_buf)); //直接网套接字里写数据就行了
```

4. 接收数据
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
    ERR_EXIT("read");
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
    3. 创建套接字并绑定ip地址
    ```
    int sockfd;
    if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
      ERR_EXIT("socket");

    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5188);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0) // "专门协议地址=>通用协议地址"使用小括号类型转换就好了
        ERR_EXIT("bind");
    ```
    
    4. 创建套接字并根据ip地址连接
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

12. 使套接字处于监听状态
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
25. [回射服务器中使用shutdown解决关闭后不能读取的问题](https://github.com/orris27/orris/blob/master/network/socket/codes/echo/readme.md)
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
