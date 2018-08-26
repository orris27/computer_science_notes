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


## 2. 接口
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
1. 创建套接字
```
int socket(int domain, int type, int protocol); // 创建1个套接字用于通信 
// domain:用哪个协议族=>PF_INET(老师推荐的..)/AF_INET
// type:套接字类型
// protocol:协议类型=>0/IPPROTO_TCP(0表示让内核自己选择协议,而实际上如果使用AF_INET+SOCK_STREAM的话,已经就是TCP协议了)
// 返回值为套接字描述符=> <0表示创建失败
```
2. 读取套接口缓冲区
```
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
// recv只能用于套接字的IO,而read可以获取其他IO
// flags可以处理MSG_OOB
// MSG_PEEK:读取套接口缓冲区的数据,但不清除缓冲区
// 错误处理:返回值<0=>异常
```
3. 写入套接口缓冲区
```
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```
4. 获取套接字的名字(可以获取自身的地址)
```
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);//获取自身的ip地址
int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);//获取对方的ip地址(套接字必须连接成功)
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


### 3-3. listen
`man listen`查看帮助
1. 函数
```
int listen(int sockfd, int backlog);
// sockfd:socket函数返回的套接字
// backlog: 规定内核为此套接字排队的最大连接个数
// 返回值:成功0,失败-1
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

