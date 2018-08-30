查看帮助:`man ipc`
## 1. IPC
### 1-1. 对象数据结构
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
### 1-2. 接口
1. convert a pathname and a project identifier to a System V IPC key.可以用填在创建IPC对象的key参数中
```
key_t ftok(const char *pathname, int proj_id);
```

## 2. System V
### 2-1. 消息队列
#### 2-1-1. 结构
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

#### 2-1-2. 接口
1. 创建/打开消息队列
    1. 参数
        1. key:关键字.可以理解为消息的名字,是整型
            1. IPC_PRIVATE: 创建1个新的消息队列
                1. 其他进程不能共享,除非保留msqid
                2. 后面的msgflg可以只跟权限
        2. msgflg:创建的选项.9个权限标志.
            1. 0666:权限(这里填0666,实际权限要和umask进行处理,所以0022的umask会得到0644的权限)
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



### 2-2. 共享内存
#### 2-2-1. 结构体
查看帮助:`man shmctl`
```
struct shmid_ds {
    struct ipc_perm shm_perm;   //
    size_t          shm_segsz;  //共享内存段的大小
    time_t          shm_atime;  //最后一次映射的时间
    time_t          shm_dtime;  //最后一次解除连接的时间
    time_t          shm_ctime;  //最后一次改变的时间
    pid_t           shm_cpid;   //创建者的pid
    pid_t           shm_lpid;   //脱离共享内存的pid
    shmatt_t        shm_nattch; //当前共享内存被连接的
    ...
};

```
#### 2-2-2. 接口
1. 映射文件/设备空间到共享内存区
    1. 参数
        1. addr:映射到哪个地址.通常为NULL(让系统自己选择地址)
        2. 映射的内容
            1. fd:文件描述符
            2. offset:偏移量
            3. len:长度
        3. prot:保护方式
            1. PROT_READ:页面(内存中的页单位,常为4k)可读
            2. PROT_WRITE:页面可写
            3. PROT_EXEC:页面可执行
            4. PROT_NONE:页面不可访问
        4. flags:映射的标志
            1. MAP_SHARED:变动是共享的.对映射的内存进行变动会被其他进程看到,并写回到文件
            2. MAP_PRIVATE:变动是私有的.其他进程看不到,而且也不会写入到文件中
            3. MAP_FIXED:如果我们指定addr,就自动对齐到给定内存中页的整数倍(内存分配是按照页为单位分配)
            4. MAP_ANONYMOUS:建立匿名映射区,不涉及文件.只能亲缘关系的进程使用
    2. 返回值:映射后在内存中的地址
    3. 注意
        1. mmap不能超过文件大小
        2. mmap实际可用的空间是页单位的,即4k.但最后写入到文件的却受文件大小限制(参考注意点1)
            1. 例子:假如我们写入10个学生的信息给原来只能放5个学生的文件,这时候让该程序睡5秒;此时另一个程序读,发现能读10个;但前面的程序执行完后就又只能读5个了
```
void *mmap(void *addr, size_t length, int prot, int flags,int fd, off_t offset);
```
2. 取消mmap函数建立的映射
    1. 参数
        1. addr:地址.必须是起始地址=mmap的返回值
        2. length:长度
```
int munmap(void *addr, size_t length);
```

3. 同步共享内存区的数据到实体文件中
    1. 内核
    2. 参数
        1. addr & length:同munmap
        2. flags:
            1. MS_ASYNC:执行异步写
            2. MS_SYNC:执行同步写.程序必须等到同步写完后才能向下执行
            3. MS_INVALIDATE:使高速缓冲区的数据失效
```
int msync(void *addr, size_t length, int flags);
```

4. 创建/访问共享内存端
    1. 使用:和msgctl一样
    2. 参数
        1. size:共享内存的大小
```
int shmget(key_t key, size_t size, int shmflg);
```
5. 连接共享内存段到进程的地址空间
    1. 参数
        1. shid:共享内存标志
        2. shmaddr:连接的地址.通常为NULL,内核自动选择
        3. shmflg:
            1. SHM_RND
                1. 如果shmaddr不为NULL且设置了SHM_RND,则连接的地址自动向下调整为shmaddr-(shmaddr%SHMLBA)
                2. 如果shmaddr不为NULL且没有设置SHM_RND,则连接的地址=shmaddr
            2. SHM_RDONLY:只读共享内存.通常为0,表示可读写
```
void *shmat(int shmid, const void *shmaddr, int shmflg);
```
6. 脱离共享内存段,从进程中脱离
    1. 参数
        1. shmaddr=shmat的返回值
```
int shmdt(const void *shmaddr);
```
7. 控制共享内存段
    1. 使用:和msgctl一样

```
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
```

### 2-3. 信号量
#### 2-3-1. 结构体
1. 信号量集的数据结构
```
struct semid_ds {
    struct ipc_perm sem_perm;  
    time_t          sem_otime; //最后一次执行P,V操作的时间
    time_t          sem_ctime; //信号量集最后一次改变的时间
    unsigned long   sem_nsems; //信号量集中的信号量个数
};

```
#### 2-3-2. 接口
1. 创建/打开信号量集
    1. 使用:与msgget一样
    2. 参数
        1. nsems:信号量集中信号量的个数 
```
int semget(key_t key, int nsems, int semflg);
```
2. 控制信号量集
    1. 参数
        1. semnum:信号量集中信号量的序号
        2. cmd:采取的动作.
            1. SETVAL:设置信号量中的计数值
            2. GETVAL:信号量中的计数值.关心semnun中的val值
            3. IPC_RMID:删除信号量集
            4. IPC_STAT:与消息队列中的IPC_STAT一样的用法
```
//下面的结构体要我们自己定义
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};


int semctl(int semid, int semnum, int cmd, ...);
```


3. PV操作
    1. 参数
        1. sops:信号量集中的哪些信号量进行操作,并存放操作内容
            1. sem_num:信号量的编号.对哪个信号量操作
            2. sem_op:+1(P操作),-1(V操作).当然也可以是-2,+2等
            3. sem_flg:操作选项.填0即可
                1. IPC_NOWAIT:没有可用资源就直接报错
                2. SEM_UNDO:当1个进程终止后就会撤销这个进程的所有PV操作.相当于这个进程对信号量的计数值毫无影响
        2. nsops:对多少个信号量操作
```
struct sembuf{
    short sem_num;
    short sem_op;
    short sem_flg;
};

int semop(int semid, struct sembuf *sops, size_t nsops);
```



## 3. POSIX
需要在连接的时候指定`-lrt`参数.查看帮助:`man 7 mq_overview`
### 3-1. 消息队列
#### 3-1-1. 接口
1. 创建/打开1个消息队列
    1. 参数
        1. name:消息队列的名字
            1. 必须以`/`开头,后面不能有其他`/`
            2. 长度不能超过NAME_MAX
        2. oflag:类似open函数,可以是O_RDONLY,O_WRONLY,O_RDWR等
        3. mode: 如果oflag指定O_CREAT,需要设置mode
```
mqd_t mq_open(const char *name, int oflag);
mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
```
2. 关闭1个消息队列
    + 不删除文件(文件是指挂载到`/dev/mqueue`后,该目录下面对应消息队列的文件)
    + 程序结束后也会自动关闭消息队列
```
int mq_close(mqd_t mqdes);
```
3. 删除1个消息队列的连接数,如果连接数变为0,就删除消息队列的实体文件
```
int mq_unlink(const char *name);
```
4. 获取消息队列属性到attr
```

struct mq_attr {
    long mq_flags;      // 是否阻塞
    long mq_maxmsg;     // 消息队列的消息个数的最大值
    long mq_msgsize;    // 消息队列中单个消息字节数的最大值
    long mq_curmsgs;    // 消息队列中当前消息个数
};


int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
```
5. 设置消息队列属性,返回原来的属性
```
int mq_setattr(mqd_t mqdes, const struct mq_attr *newattr,struct mq_attr *oldattr);
```
6. 发送POSIX消息
    1. 参数
        1. msg_ptr:消息内容.虽然是`char*`,但和缓冲区一样,我们可以传递甚至结构体过去
        2. msg_len:消息的字节长度
        3. msg_prio:优先级(>=0的数).优先级最高(且最早进入队列)的会先被接收
```
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio);
```
7. 接收POSIX消息
    1. 参数:指定的单个消息字节长度必须等于消息的最大字节长度,其他同发送POSIX消息的函数
    2. 获取单个消息的最大字节数
        1. 获取POSIX消息队列的属性
        2. 提取消息的最大字节数
        3. 使用该单个消息队列的最大字数
```
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio);
```

8. 注册消息发送/接收的通知事件
    1. 消息队列中获得/删除消息时进程是不知道的
    2. 参数
        1. sevp:通知事件的结构
            1. NULL:撤销已注册的实际那
            2. 
    3. 通知方式(下面2个选1个)
        1. 产生1个信号
        2. 创建1个线程执行1个指定的函数
    4. 发生通知的条件
        1. 消息队列从无到有(消息队列中已经有消息的话就没有通知了)
        2. 消息队列从有到无
    5. 规则
        1. 任何时刻只能有1个进程可以被注册
        2. 一直通知的话:需要在信号处理函数中再次注册,并且一定要在读取数据之前(因为如果在读取之后的话,就)
```
union sigval {          /* Data passed with notification */
    int     sival_int;         /* Integer value */
    void   *sival_ptr;         /* Pointer value */
};

// man sigevent
struct sigevent {
    int          sigev_notify; //通知的方式,信号or线程
    int          sigev_signo;  //(信号)哪个信号,比如SIGUSR1
    union sigval sigev_value;  //(信号)附加参数
    void       (*sigev_notify_function) (union sigval);
    void        *sigev_notify_attributes;
                     /* Attributes for notification thread
                        (SIGEV_THREAD) */
    pid_t        sigev_notify_thread_id;
                     /* ID of thread to signal (SIGEV_THREAD_ID) */
};

int mq_notify(mqd_t mqdes, const struct sigevent *sevp);
```
9. 状态=>任何时刻只能有1个进程可以被注册
    1. QSIZE:当前所有消息的字节数和
    2. NOTIFY:通知方式
    3. SIGNO:以哪个信号通知
    4. NOTFIY_POD:通知发送给哪个进程
```
QSIZE:0          NOTIFY:0     SIGNO:0     NOTIFY_PID:0 
```
### 3-1. 共享内存
1. 创建/打开1个共享内存对象
    1. 返回值:文件描述符
```
int shm_open(const char *name, int oflag, mode_t mode);
```
2. 关闭共享内存:使用close就好了,没有`shm_close`这个函数
```
close
```
3. 修改共享内存的大小
    1. 参数
        1. fd:文件描述符
        2. length:修改后的长度
```
int ftruncate(int fd, off_t length);
```
4. 获取共享内存对象信息(fstat还可以获取普通文件的状态信息)
    1. 信息
        1. 权限
        2. 大小
```
int fstat(int fd, struct stat *statbuf);
```
5. 删除1个共享内存的连接数,如果连接数变为0,就删除共享内存的实体文件
```
int shm_unlink(const char *name);
```
6. 映射共享内存对象(参考上面的mmap内容)
```
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
```
