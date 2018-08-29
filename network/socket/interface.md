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

## 1-2. UNIX域
### 1-2-1. 地址结构
```
#define UNIX_PATH_MAX 108

struct sockaddr_un {
    sa_family_t sun_family;     /*AF_UNIX*/
    char        sun_path[UNIX_PATH_MAX]; /*pathname*/
}

// sun_family.一般使用AF_UNIX=AF_LOCAL任意一个就行了
// sun_path.地址的路径.UNIX协议通过路径来描述地址的.使用strcpy来赋值,值随便取

````


## 2. 接口
### 2-1. 地址
#### 2-1-1. 地址转换函数
32位整数用来存放ip地址.但我们习惯的ip地址是用`.`隔开的,实际对应的32位整数不得而知.因此提供了转换函数
1. 字节序转换
```
int inet_aton(const char *cp, struct in_addr *inp); // 点分ip=>网络字节序的ip输出到inp中
in_addr_t inet_addr(const char *cp); // 点分ip=>网络字节序的ip(32位整数)输出到返回值
char *inet_ntoa(struct in_addr in); // 网络字节序的ip(32位整数,struct in_addr数据结构,只有s_addr一个成员)=>点分ip
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
ssize_t recv(int sockfd, void *buf, size_t len, int flags); //tcp
// recv只能用于套接字的IO,而read可以获取其他IO
// flags可以处理MSG_OOB
// MSG_PEEK:读取套接口缓冲区的数据,但不清除缓冲区
// 错误处理:返回值<0=>异常
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, //udp
                 struct sockaddr *src_addr, socklen_t *addrlen);
//返回值:0=>不表示对方关闭连接;<0=>错误
```
3. 写入套接口缓冲区
```
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, //udp
               const struct sockaddr *dest_addr, socklen_t addrlen);

```
4. 获取套接字的名字(可以获取自身的地址)
```
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);//获取自身的ip地址
int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);//获取对方的ip地址(套接字必须连接成功)
```

#### 3-2-2. 设置套接字参数
1. 设置套接字参数:`man setsockopt`
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
1. 使套接字处于listen状态
```
int listen(int sockfd, int backlog);
// sockfd:socket函数返回的套接字
// backlog: 规定内核为此套接字排队的最大连接个数
// 返回值:成功0,失败-1
```


### 3-4. accept
`man 2 accept`查看帮助
1. 接受对方套接字的连接
```
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// sockfd: socket函数返回的套接字
// addr: 对方的套接字地址
// addrlen: 对方的套接字地址长度.=>一定要有初始值!!!可以为sizeof(peeraddr)
// 返回值: 连接的套接字,为主动套接字.(套接字描述符)
```

### 3-5. connect
1. 连接指定ip和port
```
int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);

// 返回值:成功0,失败-1
```

### 3-6 host
```
int gethostname(char *name, size_t len); 
struct hostent *gethostbyname(const char *name);
struct hostent *gethostbyname(const char *name);
```

### 3-7. 进程
1. wait
```
pid_t wait(int *wstatus);
```
2. waitpid:暂时停止当前进程,直到有信号过来或子进程结束 > [第三方介绍](https://www.cnblogs.com/baoguo/archive/2009/12/09/1619956.html)
```
pid_t waitpid(pid_t pid, int *wstatus, int options);
// pid : -1=>等待任何一个子进程退出,和wait一样
// options:0=>不用任何参数;WNOHANG=>即使没有子进程退出,我也不继续等
// 返回值: >0=>正常退出; 0=>没有已退出的子进程可以采集
```

### 3-8. I/O
#### 3-8-1. select
```
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
// nfds: 所有文件描述符的最大值+1.(比如read:{3,5,8},write:{4,9},except:{}=>9+1=10)
// readfds:读的集合,要检测有数据可读的文件描述符
// writefds:
// exceptfds:
// timeout:如果超过超时时间但没有检测到IO,就返回0
// 输出:readfds,writefds,exeptfds和timeout都是输入也是输出,调用select函数后再从他们这获取结果
void FD_CLR(int fd, fd_set *set); // 将文件描述符从集合中清除
int  FD_ISSET(int fd, fd_set *set); // 判断文件描述符是否在集合中
void FD_SET(int fd, fd_set *set); // 添加文件描述符到集合中
void FD_ZERO(fd_set *set); // 清空集合
```
##### 3-8-1-1. 事件
1. 可读
    1. 套接口缓冲区有数据可读
    2. 对方发送FIN包
    3. 如果是监听套接口,已完成连接队不为空时
    4. 套接口上发生一个错误待处理,错误可以通过getsockopt指定SO_ERROR选项来获取
2. 可写
    1. 套接口发送缓冲区有空间容纳数据
    2. 连接的写一半关闭,即我们收到RST段之后,我们再次调用write操作(当然这时也会产生SIGPIPE信号)
    3. 套接口上发生一个错误待处理,错误可以通过getsockopt指定SO_ERROR选项来获取
3. 异常
    1. 套接口存在带外数据
#### 3-8-2. poll
```
struct pollfd {
    int   fd;         /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events,返回的事件输出到这里 */ 
};

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
// fds:关心的IO以及事件
// nfds:IO的个数
// timeout:超时时间.-1表示一直等待下去
```
#### 3-8-3. epoll
1. 查看帮助:`man epoll`,`man epoll_create`,`man epoll_ctl`,`man epoll_wait`,...
2. 接口
    1. 创建epoll实例
    ```
    int epoll_create(int size);//旧版本
    int epoll_create1(int flags);//新的epoll创建方式,使用红黑树实现旧版本的哈希实现的内容
    ```
    2. 添加/移除文件描述符到epoll中进行管理
    ```
    int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    ```
    3. 检测事件发生(阻塞)
    ```
    int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    ```
## 4. UNIX
1. bind
2. accept
3. connect
4. read&write
5. unlink:"删除"套接字文件
### 4-1. 传递文件描述符
[msghdr图解](https://github.com/orris27/orris/blob/master/network/socket/codes/send-fd/msghdr.png)
1. sendmsg和recvmsg只能是套接字.
2. 传递
3. 查看帮助:`man CMSG`
```
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);

struct msghdr {
    void         *msg_name;       //发送/接收的套接口地址,一般放NULL.(只有在数据报套接口时才需要填写)
    socklen_t     msg_namelen;    //发送/接收的套接口地址长度.如果发送/接收套接口的地址是NULL的话,这里也填0
    struct iovec *msg_iov;        //I/O向量数组.指向"struct iovec"类型数据.
    size_t        msg_iovlen;     //I/O向量数组中有多少个元素.
    void         *msg_control;    //传递文件描述符所需要的辅助数据缓冲区数组.如果传递的是文件描述符,则需要提供控制信息(普通的数据不需要)
    size_t        msg_controllen; //传递文件描述符的辅助数据缓冲区的元素大小.sizeof(xxx.msg_control).因为msg_controllen是辅助缓冲区本身的大小,而不是辅助缓冲区的个数
    int           msg_flags;      //辅助数据接收的选项,默认填0就好了
};

struct iovec {
    void  *iov_base;              //缓冲区
    size_t iov_len;               //缓冲区的大小
};

//解决填充字节:CMSG_FIRSTHD(),CMSG_NXTHDR(获取下一个辅助数据的指针)
//CMSG_LEN不包含结构体间的填充字节,但包含结构体内的填充字节.CMSG_LEN就是cmsg_len的值
//辅助数据的结构体(1个结构体内会有填充字节,2个结构体之间也有填充字节)
struct cmsghdr {
    size_t cmsg_len;    //包括cmsg_len在内整个结构体的长度.如果只是1个文件描述符的话,cmsg_len=16
    int    cmsg_level;  //(SOL_SOCKET)
    int    cmsg_type;   //cmsg_level+cmsg_type表示要传递的类型.(SCM_RIGHTS,上面2个组合表示要传递文件描述符)
/* followed by
    unsigned char cmsg_data[];*/ //4个字节的文件描述符写在这里.如果要传递多个文件描述符,直接填充到后面就可以了
};



struct cmsghdr *CMSG_FIRSTHDR(struct msghdr *msgh);//获取辅助数据的第一个指针
struct cmsghdr *CMSG_NXTHDR(struct msghdr *msgh, struct cmsghdr *cmsg);//获取辅助数据的下一个指针
size_t CMSG_ALIGN(size_t length);
size_t CMSG_SPACE(size_t length);//参数为文件描述符,输出是整个辅助数据占用的长度
size_t CMSG_LEN(size_t length);//提供数据的长度,就能提供辅助数据的长度.如p_cmsg->cmsg_len = CMSG_LEN(sizeof(send_fd))
unsigned char *CMSG_DATA(struct cmsghdr *cmsg);
// p_fds = (int*)CMSG_DATA(p_cmsg); // 这里的p_cmsg是指向辅助数据数组的第一个元素的指针,通过CMSG_FIRSTHDR获得
// *p_fds = send_fd;
```
## 5. socketpair
1. pipe是半双工的,只能父子进程/兄弟进程使用共享描述符来通信
2. socketpair也只能父子进程/兄弟进程使用共享描述符来通信,是比pipe多了全双工的功能
3. 创建全双工的流管道(一个套接字对,写入A后从B中可读取,写入B后从A中可读取),返回2个文件描述符.`sv[0]`和`sv[1]`都是既可以读都可以写
```
int socketpair(int domain, int type, int protocol, int sv[2]);
// 创建1个
// domain:协议家族(UNIX域的协议)
// type:套接字类型,比如说SOCK_STREAM这样的流协议
// protocol:协议类型.填0,让系统自己根据协议家族和套接字类型来判断
// sv:返回套接字对
// 返回值:-1失败,0成功
```

