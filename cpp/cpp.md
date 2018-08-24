    
## 1. Makefile
### 1-1. 使用
1. 使用模板的Makefile
2. 对于要编译的`xx.c`文件,在BIN处写上`BIN=xx`,并保存Makefile文件
3. make
4. `./xx`运行
```
vim Makefile
#########################################################
.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=
all:$(BIN)
$.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)
#########################################################
```
## 2. 进程
### 2-1. 函数
1. 创建子进程
```
pid_t pid;
pid = fork();
if (pid == 0)
{
	// 子进程处理内容
    exit(EXIT_SUCCESS);
}
else
{
    // 主进程处理内容
}
```
