#include "fifo.h"

// 定义学生的结构体
typedef struct{
    // name[32]
    char name[32];
    // age
    int age;
} Student;
int main()
{
    // 初始化共享内存结构变量
    key_t key = ftok(".",'s');
    Shmfifo *p_fifo = fifo_init(key,sizeof(Student),3);
    
    // 存放5个学生到共享内存中
    // 1. 定义学生结构变量
    Student stu;
    // 2. 清洗变量
    memset(&stu,0,sizeof(Student));
    // 3. for(i:0=>5)
    for(int i=0;i<5;++i)
    {
        // 填充学生结构变量
        strcpy(stu.name,"orris");
        stu.age = 20+i;
        // 存放到共享内存中
        fifo_produce(p_fifo,(char*)&stu);
        printf("put name=%s age=%d\n",stu.name,stu.age);
    }
    return 0;

}
