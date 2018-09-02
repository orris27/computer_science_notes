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
1. mmap
    1. 映射文件/设备空间到物理内存的共享内存区
    2. 参数
        1. addr:映射到哪个地址.通常为NULL(让系统自己选择地址)
        2. 映射的内容
            1. fd:文件描述符
            2. offset:偏移量
            3. len:长度
        3. prot:映射区的权限.映射区的权限必须小于打开文件的读写权限
            1. PROT_READ:页面(内存中的页单位,常为4k)可读
            2. PROT_WRITE:页面可写
            3. PROT_EXEC:页面可执行
            4. PROT_NONE:页面不可访问
        4. flags:映射的标志.映射区所做的修改是否反应到物理设备上
            1. MAP_SHARED:变动是共享的,映射区所做的修改会影响到物理设备.对映射区进行变动会被其他进程看到,并写回到文件
            2. MAP_PRIVATE:变动是私有的,映射区所做的修改不会影响到物理设备.其他进程看不到,而且也不会写入到文件中
            3. MAP_FIXED:如果我们指定addr,就自动对齐到给定内存中页的整数倍(内存分配是按照页为单位分配)
            4. MAP_ANONYMOUS:建立匿名映射区.
                + 不涉及文件=>文件描述符参数填-1
                + len:自己填
                + 只能亲缘关系的进程使用
    3. 返回值:映射后在内存中的地址
    4. 注意
        1. mmap不能超过文件大小.否则发生`Bus error (core dumped)`错误
        2. mmap实际可用的空间是页单位的,即4k.但最后写入到文件的却受文件大小限制(参考注意点1)
            1. 例子:假如我们写入10个学生的信息给原来只能放5个学生的文件,这时候让该程序睡5秒;此时另一个程序读,发现能读10个;但前面的程序执行完后就又只能读5个了
    5. 作用:
        1. 父子进程间通信(代码实现参考[ipc代码笔记](https://github.com/orris27/orris/tree/master/process/ipc/codes/mmap-ipc))
            1. 临时文件
                1. 创建1个临时文件
                2. 设置临时文件的大小,使临时文件可以容纳所有数据(注意点1)
                3. 映射该临时文件到进程的地址空间(必须是`MAP_SHARED`,因为父子其实是靠临时文件来通信的)
                4. 创建1个子进程
                5. 父子进程只要使用对应的进程地址空间就能通信了
            2. 匿名映射(只能适用于Linux,而不能使用在其他类UNIX系统)
                1. 映射1个匿名映射到地址空间
                2. 创建1个子进程
                3. 父子进程只要使用对应的进程地址空间就能通信了
            3. "/dev/zero"(适用于所有类UNIX系统)
                1. 打开/dev/zero文件
                2. 映射该临时文件到进程的地址空间(必须是`MAP_SHARED`,因为父子其实是靠临时文件来通信的)
                3. 创建1个子进程
                4. 父子进程只要使用对应的进程地址空间就能通信了
        2. 无血缘关系的进程间通信
            1. 临时文件
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
### 3-2. 共享内存
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
### 3-3. POSIX线程库
查看线程库版本`getconf GNU_LIBPTHREAD_VERSION`
1. 创建1个新的线程
    1. 参数
        1. thread:返回线程ID
        2. attr:设置线程的属.一般为NULL,表示使用默认属性
        3. start_routine:线程的入口地址.线程是单独的控制序列,入口地址就是这个
        4. arg:传递给线程的参数.不传的话写NULL
    2. 返回值
        1. 成功:0
        2. 失败:返回错误码(不是全局变量errno).用strerror(ret)来处理并错误退出.(而之前的handle_error中的perror检查的是全局变量的errno)
            1. 原因:线程共享全局变量,所以如果使用errno的话,所有线程会共享errno
    3. 注意
        1. 主线程结束,创建出来的线程就不能执行了=>主线程要等待新线程结束(sleep/)
        2. 主线程和新线程是交替运行的,我们可以用`usleep(20);`(微妙级别的sleep)来观察到结果
        3. 连接时要添加`-pthread`参数
```
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg);
```
2. 等待新线程执行完毕(类似于进程的waitpid)
    1. 参数
        1. thread:线程id
        2. retavl:可以直接填NULL.值是线程退出时的返回值,即pthread_exit的参数或者线程处理函数的return
    2. 返回值:通pthread_create
```
int pthread_join(pthread_t thread, void **retval);
```
3. 退出线程(类似于进程的exit)<=>线程入口函数的return(类似于进程的main函数的return)
    1. 参数
        1. retval:退出时返回得到结果,比如"ABC",NULL
```
void pthread_exit(void *retval);
```

4. 返回线程id
```
pthread_t pthread_self(void);
```
5. 取消一个执行中的线程(类似于进程的kill)
```
int pthread_cancel(pthread_t thread);
```
6. 脱离线程=>线程结束后即使主线程不调用join也能回收尸体而不变成僵尸线程
```
int pthread_detach(pthread_t thread);
```
7. 线程属性
    1. 初始化
    ```
    int pthread_attr_init(pthread_attr_t *attr);
    ```
    2. 销毁
    ```
    int pthread_attr_destroy(pthread_attr_t *attr);
    ```
    3. 获取/设置
        1. 获取/设置分离属性
            1. attr:调用初始化函数后的线程属性
            2. detachstate
                1. PTHREAD_CREATE_DETACHED:线程是分离的
                2. PTHREAD_CREATE_JOINABLE:线程不是分离的(默认)
        ```
        int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
        int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
        ```
        2. 获取/设置栈大小(通常不会自己设置,使用系统默认的就好,因为自己设置的可能会有问题)
        ```
        int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
        int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
        ```
        3. 获取/设置栈溢出保护区大小
        ```
        int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
        int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
        ```
        4. 获取/设置线程竞争范围
        ```
        int pthread_attr_setscope(pthread_attr_t *attr, int scope);
        int pthread_attr_getscope(const pthread_attr_t *attr, int *scope);
        ```
        5. 获取/设置调度策略
        ```
        int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
        int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
        ```
        6. 获取/设置继承的调度策略
        ``` 
        int pthread_attr_setinheritsched(pthread_attr_t *attr,
                                         int inheritsched);
        int pthread_attr_getinheritsched(const pthread_attr_t *attr,
                                         int *inheritsched);

        ```
        7. 获取/设置调度参数
        ```
        int pthread_attr_setschedparam(pthread_attr_t *attr,
                                       const struct sched_param *param);
        int pthread_attr_getschedparam(const pthread_attr_t *attr,
                                       struct sched_param *param);

        ```

8. 获取/设置并发级别(仅适用于混合线程模型)
    + 并发级别就是给内核的建议,多少个用户线程对应多少个LWP等.但内核到底采用不采用就是另一回事了
    + 默认值是0表示内核按照自己认为合适的级别进行映射
```
int pthread_setconcurrency(int new_level);
int pthread_getconcurrency(void);
```

9. 创建TSD的<key,value>
    1. 参数
        1. key
        2. destructor:提供销毁实际数据的函数
            + 注意:如果只是创建key而没有设置value值的话,是不会调用destructor的
```
#include <pthread.h>
// 找到1个空位置的key来创建
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
```
10. 获取/设置TSD的<key,value>的value值
```
void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
```
11. 删除TSD的<key,value>
```
int pthread_key_delete(pthread_key_t key);
```
12. 只有第一个线程进来的时候才会执行handle_once方法,而第二个线程不会调用
    + 作用:放在线程的入口函数里,可以让只有1个线程去执行.比如在线程里创建TSD的key方法(之前都是在创建线程前创建key的,所以没有多个线程都执行的这个问题)
```
int pthread_once(pthread_once_t *once_control,
    void (*init_routine)(void));
pthread_once_t once_control = PTHREAD_ONCE_INIT;

```



#### 3-3-1. POSIX线程池(自定义)
1. 结构体
```
struct task 
{
    void *(*run)(void *arg);   //指针函数（即这个任务要去做什么事情）
    void *arg;                 // 参数
    struct task *next;         // 指向下一个任务的指针
} task_t;

typedef struct threadpool //线程池结构体
{
    condition_t ready; // 任务准备就绪或者线程池销毁通知
    task_t *first;     // 任务队列头指针
    task_t *last;      // 任务队列尾指针
    int counter;       // 线程池中当前线程数
    int idle;          // 线程池中当前正在等待任务的线程数
    int max_threads;   // 线程池中最大允许的线程数
    int quit;          // 销毁线程池的时候置1
    
}threadpool_t;

void threadpool_init(threadpool_t *pool,int threads);
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg),void *arg);
void threadpool_destroy(threadpool_t *pool);


typedef struct condition{
    pthread_mutex_t pmutex;
    pthread_cond_t pcond;
}condition_t;

int condition_init(condition_t *cond);
int condition_lock(condition_t *cond);
int condition_unlock(condition_t *cond);
int condition_wait(condition_t *cond);
int condition_timedwait(condition_t *cond,const struct timespec *abstime);
int condition_signal(condition_t *cond);
int condition_broadcast(condition_t *cond);
int condition_destroy(condition_t *cond);

```



### 3-4. POSIX信号量
1. 创建1个信号量
```
// 初始化并打开1个有名的信号量
sem_t *sem_open(const char *name, int oflag);
sem_t *sem_open(const char *name, int oflag,
                mode_t mode, unsigned int value);
                
//初始化1个无名的信号量(也可以用与不同进程间通信,只要sem在共享内存区且pshared不等于0 )
int sem_init(sem_t *sem, int pshared, unsigned int value);
```
2. 执行POSIX信号量的PV操作
```
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);
```

### 3-5. 互斥锁
1. 初始化
    1. 参数
        1. mutex:互斥锁
        2. attr:填NULL就行
```
int pthread_mutex_init(pthread_mutex_t *restrict mutex,
    const pthread_mutexattr_t *restrict attr);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; # 静态的互斥锁的初始化方法


int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex); # 尝试加锁,如果加锁失败,就不阻塞,直接返回错误
int pthread_mutex_unlock(pthread_mutex_t *mutex);


int pthread_mutex_destroy(pthread_mutex_t *mutex);

```


### 3-6. 自旋锁
```
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
int pthread_spin_destroy(pthread_spinlock_t *lock);
```

### 3-7. 读写锁
```
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,
    const pthread_rwlockattr_t *restrict attr);
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```


### 3-8. 条件变量
1. 接口
```
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_init(pthread_cond_t *restrict cond,
    const pthread_condattr_t *restrict attr);
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int pthread_cond_wait(pthread_cond_t *restrict cond,
    pthread_mutex_t *restrict mutex); // 等待1个条件
int pthread_cond_timedwait(pthread_cond_t *restrict cond,
    pthread_mutex_t *restrict mutex,
    const struct timespec *restrict abstime); // 等待1个条件,有超时

int pthread_cond_signal(pthread_cond_t *cond); // 向等待的线程发起通知
int pthread_cond_broadcast(pthread_cond_t *cond); // 向等待的所有线程发起通知
```

2. 代码规范
    1. 等待条件代码
        1. while(条件为假):防止被虚假唤醒(收到信号但条件还是假)
        2. pthread_cond_wait
            1. 解锁mutex(必须在等待前锁定mutex):其他线程可以访问互斥资源(1.其他线程可以进来等待条件 2.其他进程可以进来改变条件)
            2. 等待条件,直到其他线程发起通知
            3. 重新锁定mutex
    ```
    pthread_mutex_lock(&mutex);
    while(条件为假)
        pthread_cond_wait(cond,mutex);
    修改条件
    pthread_mutex_unlock(&mutex);
    ```
    2. 发送信号的代码
        1. pthread_cond_signal
            1. 通知第一个等待的线程,如果没有线程处于等待,就忽略该通知(phread_cond_broadcast则是所有)
    ```
    pthread_mutex_lock(&mutex);
    设置条件为真
    pthread_cond_signal(cond); //更谨慎点可以:if(条件为真) pthread_cond_signal(cond);
    pthread_mutex_unlock(&mutex);
    ```

### 3-9. 文件锁(线程间不能使用,进程间能使用)

+ 借助 fcntl函数来实现锁机制.
+ 操作文件的进程没有获得锁时,可以打开,但无法执行read、write操作
2. 接口
    1. `int fcntl(int fd, int cmd, ... /* arg */ );`
        1. 参数
            1. 参数2
                1. `F_SETLK (struct flock *)`:设置文件锁（trylock）
                2. `F_SETLKW (struct flock *)`:设置文件锁（lock）W --> wait
                3. `F_GETLK (struct flock *)`:获取文件锁
                4. `struct flock`
            2. 参数3
            ```
            struct flock {
                  ...
                  short l_type;    锁的类型:F_RDLCK 、F_WRLCK 、F_UNLCK
                  short l_whence;  偏移位置:SEEK_SET、SEEK_CUR、SEEK_END 
                  off_t l_start;   起始偏移:1000
                  off_t l_len;     长度:0表示整个文件加锁.最终加锁=[偏移位置+起始偏移,偏移位置+起始偏移+长度]
                  pid_t l_pid;     持有该锁的进程ID:(F_GETLK only)
                  ...
             };
            ```

## 4. 管道
1. 函数
    1. pipe
