查看帮助:`man ipc`
## 1. IPC对象数据结构
查看帮助:`man semctl`.System V随内核持续
```
struct ipc_perm {
    key_t          __key; //选择消息队列,共享内存,信号量中的哪一个
    uid_t          uid;   //用户id
    gid_t          gid;   //组id
    uid_t          cuid;  
    gid_t          cgid;  
    unsigned short mode;  //访问权限
    unsigned short __seq; //序号
};
```


## 2. 消息队列
### 2-1. 结构
查看帮助:`man msgctl`.[消息队列结构示意图](https://img-blog.csdn.net/20140311105536625?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvYXNwbmV0X2x5Yw==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
```
struct msqid_ds {
    struct ipc_perm msg_perm;    //IPC对象
    time_t          msg_stime;   //往消息队列发送的时间
    time_t          msg_rtime;   //消息队列接收数据的时间
    time_t          msg_ctime;   //消息队列改变数据的时间
    unsigned long   __msg_cbytes; //消息队列当前字节数
    msgqnum_t       msg_qnum;     //消息队列中消息的个数
    msglen_t        msg_qbytes;   //每条消息允许的最大字节数.即MSGMNB
    pid_t           msg_lspid;    //最后1个往消息队列发送数据的进程的pid
    pid_t           msg_lrpid;    //最后1个从消息队列接收数据的进程的pid
};

```

### 2-2. 接口
1. 创建/打开消息队列
    1. 参数
        1. key:关键字.可以理解为消息的名字,是整型
            1. IPC_PRIVATE: 创建1个新的消息队列
                1. 其他进程不能共享,除非保留msqid
                2. 后面的msgflg可以只跟权限
        2. msgflg:创建的选项.9个权限标志.
            1. 0666:权限
                1. 如果填0表示按照原来的权限打开消息队列
            2. IPC_CREAT:创建/打开消息队列
            3. IPC_EXCL:创建消息队列,但如果已经存在就创建失败
    2. [msgget的流程图](http://static.oschina.net/uploads/space/2014/1117/215600_8hbw_2270918.gif)
```
int msgget(key_t key, int msgflg); 
```
2. 控制消息队列
    1. 作用
        + 改变消息队列的权限
        + 删除消息队列
    2. 参数
        1. msqid:msgget返回的id
        2. cmd:执行的动作
            1. 动作
                1. IPC_STAT:获取消息队列的状态,并报保存到buf中
                2. IPC_SET:设置消息队列的状态.通常是先获取再设置
                3. IPC_RMID:根据msgid删除消息队列.此时buf填NULL就行了,因为不传递更多信息
        3. buf:执行的动作所需要的参数,就是`struct msqid_ds`类型
```
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
```
3. 发送消息到消息队列
    1. 参数
        1. msqid
        2. msqp:指向发送的消息的指针.
            1. 内容
                1. mtype:消息类型(其实是消息的编号/名字)
                    1. 接收的时候可以根据该类型值来接收消息,而不是按顺序接收消息
                    2. 一个消息队列中可以有多个相同消息类型值的消息
                2. 实际数据存放的地址
            2. 限制
                1. 一条消息实际数据消息的大小(实际数据的大小,)<=MSGMAX
                2. 必须以long int长整型mtyp开始,表示显示类型
        3. msgsz:msgp指向的消息长度.这个长度不包含保存消息类型的那个long int长整型
        4. msgflg:发送选项
            1. IPC_NOWAIT:队列满不等待.并报错
            2. 0:队列满等待
    2. 返回值
```
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);

// man msgsnd中的参考消息结构体
struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
};
```
4. 接收来自消息队列的消息
    1. 参数
        1. msgtyp:表示接收哪种消息类型的消息.否则就按顺序接收消息
            1. msgtype>0:接收对应消息类型的消息
            2. msgtype=0:按顺序接收消息
            3. msgtype<0:接收所有消息中消息类型mtype<=|msgtype|的最小消息类型的消息,与顺序无关
        2. msgflg
            1. IPC_NOWAIT:队列中没有可读消息就不等待,返回ENOMSG错误
            2. MSG_NOERROR:消息大小超过msgsz时被截断.如果接收的消息的大小>msgsz,就截断而不是报错
            3. msgtype>且MSC_EXCEPT:接收类型不等于msgtype的第一条消息
    2. 返回:实际放到接收缓冲区里的数据的字节个数
    3. 使用:可以参考这里的[代码](https://github.com/orris27/orris/tree/master/process/ipc/codes)或者[c.md](https://github.com/orris27/orris/blob/master/template/codes/c/c.md)
```
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
```
