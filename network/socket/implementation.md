1. [回射C/S-TCP模型](https://github.com/orris27/orris/tree/master/network/socket/codes/simple-tcp)
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
read(sockfd,recv_buf,sizeof(recv_buf)); //直接网套接字里写数据就行了
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
```
int sockfd;
if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
  ERR_EXIT("socket");
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


14. 创建套接字并根据ip地址连接
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
struct sockaddr peer_addr;
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
