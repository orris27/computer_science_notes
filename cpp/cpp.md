## 1. Makefile
根据终极规则,检查依赖.如果依赖存在,就查找子规则,根据子规则检查修改时间并进行可能的更新;如果依赖不存在,就检查子规则,根据子规则的命令获得目标
### 1-1.命名格式
2个名字都可以
1. makefile
2. Makefile
### 1-2. 语法
1. 三要素
    1. 目标
    2. 依赖
        1. `.c`文件
    3. 命令
2. 规则:命令前必须是tab键
    1. 格式
    ```
    目标:依赖
        命令
    ```
    2. 最上面的规则是终极规则,必然被执行.子规则只有被需要的时候才执行
    3. 如果没有依赖的文件的话,就向下查找看是否有符合条件的规则用来生成依赖的文件.比如说没有.o文件的话,就去子规则里找能生成.o文件的子规则并执行
    4. 直接执行`make <目标>`的话,也会在命令行中运行`<命令>`
    5. "正则"匹配规则(模式规则):终极规则根据依赖查询子规则的时候,如果"正则"匹配上了,就会使用该规则
        1. 命令
            1. `$<`:当前规则中的第一个依赖.自动变量(值根据模式规则而更改)
            2. `$@`:当前规则中的目标
            3. `$^`:当前规则中的所有依赖
    ```
    %.o:%.c
    	gcc -c $< -o $@
    ```
    6. 命令
        1. 实际上命令可以是多行,只要tab就行了
        ```
        clean:
            -mkdir /aa # 如果只使用"mkdir /aa"的话,可能执行失败(比如权限不够),那样就不会向下执行了.而使用-的话,就会忽略错误,继续向下执行
            rm -f xxx
        ```

3. 变量
    1. 定义:"var=main.o add.o"
    2. 取值:`$(var)`.直接展开
    3. 种类
        1. makefile维护的变量
            1. `CC`:默认是`cc`(其实就是gcc)
            2. `CPPFLAGS`:预处理(宏替换,展开,删除注释)的时候需要的参数.可以放头文件的位置.`-I x`
            3. `CFLAGS`:编译的时候需要的参数.如`-Wall -g -c`
            4. `LDFLAGS`:链接库的时候需要的参数.如`-L -l`
        2. 用户自定义变量

4. 函数
    1. 所有的函数都有返回值.我们也就是要获取该返回值
    2. 参数直接空格写在函数后面,不同参数用逗号`,`分割
    3. 常用函数
        1. wildcard
            1. 返回值:空格分开的文件名
            2. 获取指定目录下的所有.c文件:`src=$(wildcard ./*.c)`
        2. patsubst:模式匹配替换的函数.从第3个参数中取出来,匹配第1个参数,替换成第2个参数
            1. 替换.c文件到.o文件:`obj=$(patsubst ./%.c, ./$.o, $(src))`
5. 注释:`#`
6. `make clean`
    1. 实际上make是以为要生成`clean`这个文件才去执行命令的(虽然我们的命令并不是要生成clean,而且也不会生成clean)
    2. 问题:`touch clean;make clean;`就不会执行clean下的命令了,因为makefile会认为clean这个文件已经生成了,而且是最新的
    3. 结论:申明clean为伪目标.`.PHONY:clean`
### 1-3. 例子
1. 最简单的
    1. 问题
        1. 所有的.c文件都要编译一遍,无论有没有修改
```
main:main.c add.c sub.c
	gcc main.c add.c sub.c -o main -Wall -g
```
2. 分开编译.c文件到.o文件
    1. 如果修改了一个文件,再次make的时候就只会更新这个文件的内容.
        1. 原理:比较目标和依赖的最后一次修改时间,如果不一样,就重新执行命令
```
main:main.o add.o sub.o
	gcc main.c add.c sub.c -o main -Wall -g
main.o:main.c
	gcc -c main.c
add.o:add.c
	gcc -c add.c
sub.o:sub.c
	gcc -c sub.c

```
3. 使用变量替换终极规则
```
OBJS=main.o add.o sub.o
TARGET=main
$(TARGET):$(OBJS)
	gcc main.c add.c sub.c -o main -Wall -g
main.o:main.c
	gcc -c main.c
add.o:add.c
	gcc -c add.c
sub.o:sub.c
	gcc -c sub.c
```
4. 使用"正则"匹配合并相同规则
```
OBJS=main.o add.o sub.o
TARGET=main
$(TARGET):$(OBJS)
	gcc main.c add.c sub.c -o main -Wall -g
%.o:%.c
	gcc -c $< -o $@
```
5. 使用系统维护的变量
```
OBJS=main.o add.o sub.o
TARGET=main
CC=gcc
CPPFLAGS=-I
CFLAGS=-Wall -g
$(TARGET):$(OBJS)
	$(CC) main.c add.c sub.c -o main -Wall -g
%.o:%.c
	$(CC) -c $< -o $@
```
6. 使用函数,并且用户自定义变量改成小写
```
src=$(wildcard ./*.c)
objs=$(patsubst %.c,%.o,$(src))
target=main
CC=gcc
CPPFLAGS=-I
$(target):$(objs)
	$(CC) main.c add.c sub.c -o main -Wall -g
%.o:%.c
	$(CC) -c $< -o $@
```
7. 增加clean
```
src=$(wildcard ./*.c)
objs=$(patsubst %.c,%.o,$(src))
target=main
CC=gcc
CPPFLAGS=-I
$(target):$(objs)
	$(CC) main.c add.c sub.c -o main -Wall -g
%.o:%.c
	$(CC) -c $< -o $@
clean:
	rm -f $(objs) $(target)
```
8. 申明伪目标clean
```
.PHONY:clean
src=$(wildcard ./*.c)
objs=$(patsubst %.c,%.o,$(src))
target=main
CC=gcc
CPPFLAGS=-I
$(target):$(objs)
	$(CC) main.c add.c sub.c -o main -Wall -g
%.o:%.c
	$(CC) -c $< -o $@
clean:
	rm -f $(objs) $(target)
```
### 1-2. 使用
#### 1-2-1. 单个程序
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
#### 1-2-2. 相互依赖的程序
```
.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=main
OBJS1=main.o h1.o
all:$(BIN)
$.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
main:$(OBJS1)
	$(CC) $(CFLAGS) $^ -o $@
clean:
	rm -f *.o $(BIN)
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

## 3. 库
### 3-1. 静态库
#### 3-1-1. 制作
1. 生成.o文件
2. 打包.o文件
3. 编译时指定静态库
```
tree
#------------------------------------------------------------------------------
# .
# ├── include
# │   └── calc.h
# ├── lib
# ├── main.c
# └── src
#     ├── add.c
#     └── minus.c
#------------------------------------------------------------------------------

vim include/calc.h
###################################################################################
#ifndef _CALC_H_
#define _CALC_H_
int add(int a,int b);
int minus(int a,int b);
#endif
###################################################################################


vim src/add.c
###################################################################################
#include "calc.h"

int add(int a,int b)
{
    return a+b;
}
###################################################################################


vim src/minus.c 
###################################################################################
#include "calc.h"

int minus(int a,int b)
{
    return a-b;
}
###################################################################################

vim main.c 
###################################################################################
#include <stdio.h>
#include "calc.h"

int main()
{
    int a = 1;
    int b = 2;
    printf("%d\n",add(a,b));
    return 0;
}
###################################################################################





cd src/
gcc *.c -c -I ../include
ar rcs libmycalc.a *.o # 
mv libmycalc.a ../lib
cd ..
gcc main.c lib/libmycalc.a -o main -I ./include
#gcc main.c -I ./include -L lib -l mycalc -o main 
./main
```


### 3-2. 动态库
#### 3-2-1. 制作
1. 生成位置无关的.o文件
2. 打包.o文件
3. 编译时指定动态库
```

tree
#------------------------------------------------------------------------------
# .
# ├── include
# │   └── calc.h
# ├── lib
# ├── main.c
# └── src
#     ├── add.c
#     └── minus.c
#------------------------------------------------------------------------------

cd src/
gcc -fPIC -c *.c -I ../include
gcc -shared -o libmycalc.so *.o
mv libmycalc.so ../lib/
cd ..
gcc main.c lib/libmycalc.so -o main -I ../include
#gcc main.c -I ./include -L lib -l mycalc -o main 

./main
#---------------------------------------------------------------------
# ./main: error while loading shared libraries: libmycalc.so: cannot open shared object file: No such file or directory
#---------------------------------------------------------------------

ldd main
#---------------------------------------------------------------------
# 	linux-vdso.so.1 =>  (0x00007ffff1161000)
# 	libmycalc.so => not found # 自己的库没有找到
# 	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fdca53b7000) # 标准c库
# 	/lib64/ld-linux-x86-64.so.2 (0x00007fdca5999000)
#---------------------------------------------------------------------



##########################################################################################
# 方法1:使用系统动态库路径
##########################################################################################

sudo cp lib/libmycalc.so /lib
./main
#---------------------------------------------------------------------
# 3
#---------------------------------------------------------------------
sudo rm -f /lib/libmycalc.so



##########################################################################################
# 方法2:临时/永久自定义动态库路径(修改环境变量)
##########################################################################################
export LD_LIBRARY_PATH=./lib/ # 如果添加到/etc/profile等文件中就能永久生效了
ldd main
#---------------------------------------------------------------------
#	linux-vdso.so.1 =>  (0x00007ffe09b29000)
#	libmycalc.so => ./lib/libmycalc.so (0x00007fae750a9000)
#	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fae74cc9000)
#	/lib64/ld-linux-x86-64.so.2 (0x00007fae754ad000)
#---------------------------------------------------------------------
./main 
#---------------------------------------------------------------------
# 3
#---------------------------------------------------------------------



##########################################################################################
# 方法3:永久自定义动态库路径(修改配置文件)
##########################################################################################
sudo vim /etc/ld.so.conf
########################################################################################
/home/orris/fun/c/test/lib
#########################################################################################

sudo ldconfig -v # 使配置文件生效
./main 
#---------------------------------------------------------------------
# 3
#---------------------------------------------------------------------

```
