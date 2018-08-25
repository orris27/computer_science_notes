    
## 1. 解决TCP粘包
1. 封装read和write函数,保证发送/接收的数据字节长度为定值
    1. 检查发送/接收到的长度是否等于给定的字节长度.
        1. 发送/接收到的字节长度<=write/read的返回值
        2. 如果不够的话,就一直循环直到接收到足够长度的内容
2. 自定义数据包类型
    1. 结构
        1. 真实数据的字节长度
        2. 真实数据
    2. 使用
        1. 发送:全部发送出去
        2. 接收   
            1. 获得真实数据的字节长度
            2. 获取给定字节长度的真实数据
## 2. 课堂笔记(可无视)
```
man 2 read
ssize_t read(int fd, void *buf, size_t count);

ssize_t read(int fd, void *buf, size_t count);
{
    定义剩余的字节数为变量,size_t nleft = count;
    接收到的字节数
    定义指针bufp,指向buf
    
    循环接收,只要剩余的字节数>0就接收
        读取到bufp中,使用read函数
        继续读取,如果返回值<0,并且为中断
        退出进程,如果返回值<0,并且不是中断的信号
        返回已经读取的字节数,如果返回值为0,说明对方关闭了
        移动指针
        更新剩余的字节数 
    返回定长的数值,count
}

ssize_t  writen(int  fd,  const  void  *buf,  size_t count);
{
    定义剩余的字节数为变量,size_t nleft = count;
    写入的字节数
    定义指针bufp,指向buf
    
    循环发送,只要剩余的字节数>0就发送
        读取到bufp中,使用read函数
        继续读取,如果返回值<0,并且为中断
        退出进程,如果返回值<0,并且不是中断的信号
        继续返送,如果返回值为0,说明什么都没有发送
        移动指针
        更新剩余的字节数 
    返回定长的数值,count
}



struct packet {
    数据包的字节长度;
    真实数据;
}

recvbuf=>packet类型
sendbuf=>packet类型
清空结构体的内容
n=strlen(sendbuf.buf);网络上发送的只要是整数就有字节序的问题
n=htonl(n)
writen(sockfd,&recvbuf,4+n);
发送
    字节序处理包头里的长度
    获取字符串的字节长度
    直接发送4+n字节长度的数据过去

接收
    获取真实数据的字节数量
    错误处理
    处理接收到的len长度的字节序
    接收字节流
```
