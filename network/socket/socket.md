    
## 1. 结构体
### 1-1. socket地址
查看:`man 7 ip`
1. ipv4 socket结构
```
struct sockaddr_in {
    uint8_t sin_len; # 结构体的大小
    sa_family_t sin_family; # 地址家族.socket能用在很多协议,不仅是TCP/IP协议.设置为AF_INET,说明用在ipv4协议上
    in_port_t sin_port; # 端口.2字节的无符号整数
    struct in_addr sin_addr; # ipv4地址(是主机字节序还是网络字节序?).该结构体只有1个成员.无符号的32位整数
    char sin_zero[8]; # 保留
};

/* Internet address. */
struct in_addr {
    uint32_t       s_addr;     /* address in network byte order */
};
```
2. 通用地址结构:可以用在任意协议
```
struct sockaddr {
    uint8_t sin_len;
    sa_family_t sin_family;
    char sa_data[14]; # ipv4中:14字节=2字节sin_port+4字节sin_addr+8字节sin_zero
};
```




```
struct socket
{
     socket_state  state; // socket state
     short   type ; // socket type
     unsigned long  flags; // socket flags
     struct fasync_struct  *fasync_list;
     wait_queue_head_t wait;
     struct file *file;
     struct sock *sock;  // socket在网络层的表示；
     const struct proto_ops *ops;
}


struct socket结构体的类型
enum sock_type
{
   SOCK_STREAM = 1, // 用于与TCP层中的tcp协议数据的struct socket
   SOCK_DGRAM  = 2, //用于与TCP层中的udp协议数据的struct socket
   SOCK_RAW    = 3, // raw struct socket
   SOCK_RDM    = 4, //可靠传输消息的struct socket
   SOCK_SEQPACKET = 5,// sequential packet socket
   SOCK_DCCP   = 6,
   SOCK_PACKET = 10, //从dev level中获取数据包的socket
};

struct socket 中的flags字段取值：
  #define SOCK_ASYNC_NOSPACE  0
  #define SOCK_ASYNC_WAITDATA 1
  #define SOCK_NOSPACE        2
  #define SOCK_PASSCRED       3
  #define SOCK_PASSSEC        4
```

## 2. 字节序
### 2-1. 类型

1. Big Endian:最高有效位存储在最低内存地址
2. Little Endian:最高有效位存储在最高内存地址
### 2-2. 网络字节序 vs 主机字节序
1. 网络字节序:规定使用Big Endian在网络中传输.Big Endian.
2. 主机字节序:主机存储的方式.不同主机不一样
    1. x86一般为小端字节序
    2. [查看自己电脑的字节序](https://github.com/orris27/orris/tree/master/network/socket/codes/endian-test)
3. 转换
    1. `h`:host
    2. `n`:network
    3. `s`:short
    4. `l`:long
```
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort); //可以用来转换端口号.如server_addr.sin_port = htons(5188);
uint32_t ntohl(uint32_t hostlong);
uint16_t ntohs(uint16_t hostshort);

man htonl
#------------------------------------------------------
# #include <arpa/inet.h> # 说明要包含这个头文件才能使用
#------------------------------------------------------



vim endian.c
##############################################################
#include <stdio.h>
#include <arpa/inet.h>

int main()
{
    unsigned int x = 0x12345678;
    unsigned int y = htonl(x);
    unsigned char *p = (unsigned char*)&y;
    printf("%0x %0x %0x %0x\n",p[0],p[1],p[2],p[3]);

    return 0;
}
##############################################################

make
./endian
#------------------------------------------------------
# 12 34 56 78
#------------------------------------------------------
```

## 3. 函数
### 3-1. 地址
#### 3-1-1. 地址转换函数
32位整数用来存放ip地址.但我们习惯的ip地址是用`.`隔开的,实际对应的32位整数不得而知.因此提供了转换函数
1. 接口
```
int inet_aton(const char *cp, struct in_addr *inp); // 点分ip=>网络字节序的ip输出到inp中
in_addr_t inet_addr(const char *cp); // 点分ip=>网络字节序的ip(32位整数)输出到返回值
char *inet_ntoa(struct in_addr in); // 网络字节序的ip(32位整数,struct in_addr数据结构,只有s_addr一个成员)=>点分ip
```
2. 实例
```
vim addr.c
##############################################################
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main()
{
    unsigned int inip;
    inip = inet_addr("192.168.0.100"); //转换成了网络字节序的ip
    printf("addr=%u\n",ntohl(inip));  // 转换成了主机字节序的ip

    struct in_addr ipaddr;
    ipaddr.s_addr = inip;
    printf("%s\n",inet_ntoa(ipaddr)); // 网络字节序ip=>点分ip
    return 0;
}
##############################################################

make && .addr
#----------------------------------------------------------------
# addr=3232235620
# 192.168.0.100
#----------------------------------------------------------------
```
#### 3-1-2. 地址绑定函数
```
int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
// sockfd: socket函数返回的套接字
// addr: 要绑定额的地址.(通用地址结构)
// addrlen: 地址长度
//返回值:成功0,失败-1
```

### 3-2. socket
#### 3-2-1. 创建套接字
1. 函数
```
int socket(int domain, int type, int protocol); // 创建1个套接字用于通信 
// domain:用哪个协议族=>PF_INET(老师推荐的..)/AF_INET
// type:套接字类型
// protocol:协议类型=>0/IPPROTO_TCP(0表示让内核自己选择协议,而实际上如果使用AF_INET+SOCK_STREAM的话,已经就是TCP协议了)
// 返回值为套接字描述符=> <0表示创建失败

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
// recv只能用于套接字的IO,而read可以获取其他IO
// flags可以处理MSG_OOB
// MSG_PEEK:读取套接口缓冲区的数据,但不清除缓冲区
// 错误处理:返回值<0=>异常
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);

```
2. 创建套接字
```
int sockfd;
if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
  ERR_EXIT("socket");
```

#### 3-2-2. 设置套接字参数
1. 函数:`man setsockopt`
```
int setsockopt(int sockfd, int level, int optname,
               const void *optval, socklen_t optlen);
// level: 写SOL_SOCKET就行了
// optname: 设置什么参数,包括SO_REUSEADDR
// optval: 参数的值(参数值可以是整数,也可以是结构体,所以这里要参数值的指针与参数结构体的大小)
// optlen: 参数值的大小
```
2. 允许服务器套接字重用TIME_WAIT套接字(实验中使用在bind函数之前,具体怎样不太清楚)
```
int reuse_on=1;
if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse_on,sizeof(reuse_on))<0)
  ERR_EXIT("setsockopt");
```

### 3-3. listen
`man listen`查看帮助
1. 函数
```
int listen(int sockfd, int backlog);
// sockfd:socket函数返回的套接字
// backlog: 规定内核为此套接字排队的最大连接个数
// 返回值:成功0,失败-1
```
2. 实例
```
if(listen(sockfd,SOMAXCONN)<0) // SOMAXCONN是服务器套接字允许建立的最大队列,包括未连接+已连接的队列
  ERR_EXIT("listen");
```

### 3-4. accept
`man 2 accept`查看帮助
1. 函数
```
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// sockfd: socket函数返回的套接字
// addr: 对方的套接字地址
// addrlen: 对方的套接字地址长度.=>一定要有初始值!!!可以为sizeof(peeraddr)
// 返回值: 连接的套接字,为主动套接字.(套接字描述符)
```
2. 实例
```
struct sockaddr peer_addr;
socklen_t peer_len = sizeof(peer_addr); // 使用accept获得对方的套接字等信息时,对方套接字的长度一定要初始化
int conn_sockfd;
if ((conn_sockfd = accept(sockfd,(struct sockaddr*)&peer_addr,&peer_len))<0)
  ERR_EXIT("accept");
```

### 3-5. connect
1. 函数
```
int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);

// 返回值:成功0,失败-1
```

2. 实例
```
if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0)
  ERR_EXIT("connect");
```

## 4. 套接字类型
1. SOCK_STREAM流式套接字(TCP)
    1. 提供面向连接的,可靠的数据传输服务等
2. SOCK_DGRAM数据报式套接字
    1. 提供无连接的,不可靠的数据传输服务等
3. SOCK_RAW原始套接字
    1. 将应用层的数据直接封装成ip层能理解的协议格式

## 5. 实例
1. [回射C/S-TCP模型](https://github.com/orris27/orris/tree/master/network/socket/codes/simple-tcp)
2. ipv4地址
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
