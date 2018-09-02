## 1. 帮助
查看帮助:`man epoll`,`man epoll_create`,`man epoll_ctl`,`man epoll_wait`,...

## 2. 结构体
```
typedef union epoll_data {
    void    *ptr;           // 用途:1.封装fd和fd的回调函数(直接用来处理fd)
    int      fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

struct epoll_event {
    uint32_t     events;    // 事件:EPOLLIN,EPOLLOUT,EPOLLERR
    epoll_data_t data;      // 数据:ptr/fd/u32/u64
};
```

## 3. 接口
1. 创建epoll实例
    1. `epoll_create`
        1. 参数
            1. size:将来可能会需要多少个文件描述符,只是给内核的建议
        2. 返回值  
            1. epoll的红黑树
    2. `epoll_create1`:新的epoll创建方式,使用红黑树实现旧版本的哈希实现的内容
2. 添加/移除文件描述符到epoll中进行管理:epoll_ctl
    1. 参数
        1. epfd:
        2. op
            1. EPOLL_CTL_ADD
            2. EPOLL_CTL_MOD
            3. EPOLL_CTL_DEL
        3. fd:添加/移除的文件描述符

        4. event
            1. 用来在epoll_wait里返回
            2. 结构:struct epoll_event
                1. data.fd:就是第3个参数的fd
                
3. 检测事件发生(阻塞):epoll_wait
    1. 参数
        1. epfd
        2. events
            1. 结构:struct epoll_event
            2. 数组
            3. 传出参数
        3. maxevents:events数组的元素个数
    2. 返回值:
        1. 成功:有多少个文件描述符
        2. 失败:-1,errno
        
