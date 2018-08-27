## 1. 字节序
### 1-1. 类型
1. Big Endian:最高有效位存储在最低内存地址
2. Little Endian:最高有效位存储在最高内存地址
### 1-2. 网络字节序 vs 主机字节序
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



## 2. 套接字类型
1. SOCK_STREAM流式套接字(TCP)
    1. 提供面向连接的,可靠的数据传输服务等
2. SOCK_DGRAM数据报式套接字
    1. 提供无连接的,不可靠的数据传输服务等
3. SOCK_RAW原始套接字
    1. 将应用层的数据直接封装成ip层能理解的协议格式

## 3. 结构体 & 接口
> [UNIX网络编程结构体和接口](https://github.com/orris27/orris/blob/master/network/socket/interface.md)

## 4. 实例
> [UNIX网络编程功能实现](https://github.com/orris27/orris/blob/master/network/socket/implementation.md)


## 5. 超时设置
> [超时解决](https://coggle.it/diagram/W4LEu4dcxhLSLIc6/t/io)
### 5-1. alarm
```
void handle_sigalrm(int sig)
{
    //SIGALRM信号,不用做处理,也会关闭read
    return 0;
}
signal(SIGALRM,handle_sigalrm);
//给定时间是5秒,超过时间就会发出SIGALRM信号,默认取消read
alarm(5);
int ret = read(fd,buf,sizeof(buf));
if (ret == -1 && errno == EINTR)
    errno = ETIMEDOUT;
else if (ret >= 0)
{
    //如果read出现结果,就取消掉之前设置的闹钟
    alarm(0);
}
```


## 6. UNIX域
查看帮助:`man unix`.注意不要包含<netinet.h>,而是<sys/un.h>
### 6-1. 地址结构
```
#define UNIX_PATH_MAX 108

struct sockaddr_un {
    sa_family_t sun_family;     /*AF_UNIX*/
    char        sun_path[UNIX_PATH_MAX]; /*pathname*/
}

// sun_family.一般使用AF_UNIX=AF_LOCAL任意一个就行了
// sun_path.地址的路径.UNIX协议通过路径来描述地址的.使用strcpy来赋值,值随便取
//      会在指定的字符串下面创建同名文件=>ll一下发现是s文件,即套接字文件
````
