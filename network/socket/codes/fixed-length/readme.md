    
## 1. 定长包
封装read和write函数
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
    结束循环,如果返回值为0,说明对方关闭了
    移动指针
    更新剩余的字节数
}
```
