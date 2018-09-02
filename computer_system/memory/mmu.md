## 1. PCB
1. 文件描述符表
    + 目前可以理解为1个数组
2. 描述虚拟地址空间的信息
    + MMU维护,而PCB保存
3. 当前控制终端的信息
4. 当前工作目录
5. 进程id
6. 用户id和组id
7. umask掩码
8. 信号相关的信息
    1. 未决信号集
    2. 阻塞信号集(信号屏蔽字)
9. 进程状态
    1. 分类
        1. 初始态
        2. 就绪
            + 都准备好了,就等待CPU时间片
        3. 运行态
        4. 挂起态
            + 等待除了CPU以外的其他资源.这个时候进程会主动放弃CPU
            + 比如说"阻塞I/O"
        5. 终止态
    2. 注意:初始态和就绪态有时会被认为是1种状态
10. 进程切换时需要保存和恢复的一些CPU寄存器
11. 会话(session)和进程组
    1. 会话
    2. 进程组:将多个功能相同或相似的进程组成1个进程组,方便管理
12. 进程可以使用的资源上限
    1. 查看:`ulimit -a`
    2. 内容
        1. 栈大小
        2. 最大文件描述符数