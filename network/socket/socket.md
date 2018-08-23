    
## 1. 结构体
### 1-1. socket地址
查看:`man 7 ip`
1. ipv4 socket结构
```
struct sockaddr_in {
    uint8_t sin_len; # 结构体的大小
    sa_family_t sin_family; # 地址家族.socket能用在很多协议,不仅是TCP/IP协议.设置为AF_INET,说明用在ipv4协议上
    in_port_t sin_port; # 端口.2字节的无符号整数
    struct in_addr sin_addr; # ipv4地址.该结构体只有1个成员.无符号的32位整数
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
uint16_t htons(uint16_t hostshort);
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

## 3. 方法
### 3-1. 地址转换函数
32位整数用来存放ip地址.但我们习惯的ip地址是用`.`隔开的,实际对应的32位整数不得而知.因此提供了转换函数
1. 
```
#include <netinet/in.h>
#include <arpa/inet.h>

int inet_aton(const char *cp, struct in_addr *inp); // 点分ip=>网络字节序的ip输出到inp中
in_addr_t inet_addr(const char *cp); // 点分ip=>网络字节序的ip(32位整数)输出到返回值
char *inet_ntoa(struct in_addr in); // 网络字节序的ip(32位整数,struct in_addr数据结构,只有s_addr一个成员)=>点分ip

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

## 4. 套接字类型
1. SOCK_STREAM流式套接字
    1. 提供面向连接的,可靠的数据传输服务等
2. SOCK_DGRAM数据报式套接字
    1. 提供无连接的,不可靠的数据传输服务等
3. SOCK_RAW原始套接字
    1. 将应用层的数据直接封装成ip层能理解的协议格式
