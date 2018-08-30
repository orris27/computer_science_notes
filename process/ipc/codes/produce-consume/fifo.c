#include "fifo.h"

Shmfifo* fifo_init(key_t key,unsigned int block_size,unsigned int block_total)
{
    // (共享内存结构宏观上控制共享内存,而共享内存首部结构控制实际数据)
    // 定义共享内存首部结构变量
    Shmhead *p_head;
    // 构造共享内存结构变量
    Shmfifo *p_fifo = malloc(sizeof(Shmfifo));
    // 清洗变量
    memset(p_fifo,0,sizeof(Shmfifo));
    // 打开共享内存(+错误处理)
    int shmid = shmget(key,0,0);
    // 如果打开共享内存失败
    if(shmid == -1)
    {
        // 创建共享内存:大小是首部结构大小+块数目*块大小+错误处理
        shmid = shmget(key,sizeof(Shmhead)+block_size*block_total,0666|IPC_CREAT);
        if(shmid == -1)
            ERR_EXIT("shmget");
        // 连接共享内存
        p_head = (Shmhead*)shmat(shmid,NULL,0);
        if(p_head == (void*)-1)
              ERR_EXIT("shmat");
        // 填充共享内存结构变量
        // (仓库有限,1个位置要么生产要么消费)
        // 1. 赋值共享内存id
        p_fifo->shmid = shmid;
        // 2. 赋值共享内存首部指针
        p_fifo->p_head = p_head;
        // 3. 赋值有效负载的指针(直接首部指针+1就行了,因为指针的+1会根据结构大小而改变)
        p_fifo->p_payload = (char*)(p_head + 1);
        // 4. 创建并赋值"当前位置是否被占用"(sem_mutex)的信号量集,初始化为1
        p_fifo->sem_mutex = sem_create(key);
        sem_setval(p_fifo->sem_mutex,1);
        // 5. 创建并赋值"仓库还能生产多少"(sem_produce)的信号量集,初始化为块数目
        p_fifo->sem_produce = sem_create(key+1);
        sem_setval(p_fifo->sem_produce,block_total);
        // 6. 创建并赋值"仓库还能消费多少"(sem_consume)的信号量集,初始化为0
        p_fifo->sem_consume = sem_create(key+2);
        sem_setval(p_fifo->sem_consume,0);
        // 7. 填充首部结构信息
        // 7-1. 填充块大小
        p_fifo->p_head->block_size = block_size;
        // 7-2. 填充块数目
        p_fifo->p_head->block_total = block_total;
        // 7-3. 填充第几块可以读
        p_fifo->p_head->rd_index = 0;
        // 7-4. 填充第几块可以写
        p_fifo->p_head->wr_index = 0;
    }
    // 如果打开共享内存成功(不能重新赋值/填充结构体)
    else
    {
        // 连接共享内存
        p_head = (Shmhead*)shmat(shmid,NULL,0);
        if(p_head == (void*)-1)
              ERR_EXIT("shmat");
        // 填充共享内存结构变量
        // (仓库有限,1个位置要么生产要么消费)
        // 1. 赋值共享内存id
        p_fifo->shmid = shmid;
        // 2. 赋值共享内存首部指针
        p_fifo->p_head = p_head;
        // 3. 赋值有效负载的指针(直接首部指针+1就行了,因为指针的+1会根据结构大小而改变)
        p_fifo->p_payload = (char*)(p_head + 1);
        // 4. 打开"当前位置是否被占用"(sem_mutex)的信号量集,初始化为1
        p_fifo->sem_mutex = sem_open(key);
        // 5. 打开"仓库还能生产多少"(sem_produce)的信号量集,初始化为块数目
        p_fifo->sem_produce = sem_open(key+1);
        // 6. 打开"仓库还能消费多少"(sem_consume)的信号量集,初始化为0
        p_fifo->sem_consume = sem_open(key+2);
    }
    // 返回共享内存的指针
    return p_fifo;
}


/*
 * 生产
 */
void fifo_produce(Shmfifo *p_fifo, char *buf)
{
    // (假定生产的产品(缓冲区)的大小=块大小)
    // 申请"仓库还能生产"的资源
    sem_p(p_fifo->sem_produce);
    // 申请"占用位置"的资源
    sem_p(p_fifo->sem_mutex);
    // 生产产品:在payload后面第wr_index块的位置写入数据
    // 1. 计算写入位置
    char *pos = p_fifo->p_payload+p_fifo->p_head->wr_index*p_fifo->p_head->block_size;
    // (缓冲区一般都用char*来处理,所以这里的写入位置其实是char*这个指针)
    // 2. 拷贝(写入)数据到写入位置
    memcpy(pos,buf,p_fifo->p_head->block_size);
    // 3. 更新wr_index:环形更新取余
    p_fifo->p_head->wr_index = (p_fifo->p_head->wr_index+1)%(p_fifo->p_head->block_total);
    // 归还"占用位置"的资源
    sem_v(p_fifo->sem_mutex);
    // 归还"仓库还能消费"的资源
    sem_v(p_fifo->sem_consume);
}

/*
 * 消费
 */
void fifo_consume(Shmfifo *p_fifo, char *buf)
{
    // (假定生产的产品(缓冲区)的大小=块大小)
    // 申请"仓库还能消费"的资源
    sem_p(p_fifo->sem_consume);
    // 申请"占用位置"的资源
    sem_p(p_fifo->sem_mutex);
    // 消费产品:在payload后面第rd_index块的位置写入数据
    // 1. 计算写入位置
    char *pos = p_fifo->p_payload+p_fifo->p_head->rd_index*p_fifo->p_head->block_size;
    // (缓冲区一般都用char*来处理,所以这里的写入位置其实是char*这个指针)
    // 2. 拷贝(写入)数据到写入位置
    memcpy(buf,pos,p_fifo->p_head->block_size);
    // 3. 更新rd_index:环形更新取余
    p_fifo->p_head->rd_index = (p_fifo->p_head->rd_index+1)%(p_fifo->p_head->block_total);
    // 归还"占用位置"的资源
    sem_v(p_fifo->sem_mutex);
    // 归还"仓库还能生产"的资源
    sem_v(p_fifo->sem_produce);
}
    
/*
 * destroy
 */
    // 删除3个信号量集
    // 删除共享内存
    // 1. 解除共享内存连接
    // 2. 删除共享内存
    // 释放共享内存结构变量
