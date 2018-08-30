#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "sem.h"

#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)

typedef struct {
    unsigned int block_size;  // 块大小
    unsigned int block_total; // 块数目
    unsigned int rd_index;    // 第几块可以读(消费)
    unsigned int wr_index;    // 第几块可以写(生产)

}Shmhead;

typedef struct {
    Shmhead *p_head; // 共享内存头部指针
    char *p_payload; // 有效负载的起始位置

    int shmid;       // 共享内存id
    int sem_mutex;   // 互斥用的信号量集id
    int sem_produce; // "仓库还能生产"的信号量集id
    int sem_consume; // "仓库还能消费"的信号量集id
    
}Shmfifo;



Shmfifo* fifo_init(key_t key,unsigned int block_size,unsigned int block_total);
void fifo_produce(Shmfifo *p_fifo, char *buf);
void fifo_consume(Shmfifo *p_fifo, char *buf);

