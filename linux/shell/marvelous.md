## 1. 错误提醒
如果执行失败,要在stderr中给出错误提醒,并且有必要的话要返回错误代码并退出程序
### 1-1. `&&`, `||`
```
cd $LOG_DIR || {
  echo "Cannot change to necessary directory." >&2
  exit 1
}
```
### 1-2. 命令行参数的提醒
```
if [ $# -ne 1  ]
then
  echo "*****"
	echo "Usage:source distribute.sh xxx"
  echo "*****"
  exit 1
fi
```


## 2. 每一句话都是有前提成立
比如只有正确cd进目录才能执行,只有文件存在才能执行语句
### 2-1. cd
```
cd $LOG_DIR || {
  echo "Cannot change to necessary directory." >&2
  exit 1
}
```
### 2-2. `make`成功后才能执行`make install`
```
sudo make && sudo make install
```

## 3. 版权信息
```
# Date:      16:29 2012-3-30
# Author:    Created by xxx
# Mail:      xxx@qq.com
# Function:  This script function is ...
# Version:   4.2.46
```
## 4. 处理某条命令的输出
### 4-1. 管道
输出为任意行
+ 空的1行和完全空是不一样的.在命令行中就是,前者两个`[root@172-19-28-82 www_check]#`会有空隙,后者没有
### 4-2. 重定向到文件
输出为任意行
### 4-3. 直接比较
输出确定为1行,不能为0行
### 4-4. 赋值到变量
输出确定为1行,不能为0行

## 5. 实战
### 5-1. MySQL多实例脚本
> https://github.com/orris27/orris/blob/master/database/mysql/mysqld_multi.md
### 5-2. rsync服务端脚本(参考MySQL多实例脚本)
1. kill
+ 不推荐`pkill`(如果使用,需要注意自己的脚本文件是否会被kill掉),而是使用pid文件的`kill`
+ `kill -USR2`不会删除PID文件,需要手动删除
+ `kill`会删除PID文件,但需要时间
2. 使用pid文件需要判断pid文件是否存在
3. 如果使用`case`来判断`[start|stop|restart]`的话,就不用判断`$#`了
4. 判断服务是否已经启动有很多种方法
+ 特殊文件(推荐)=>start检查文件是否存在;stop删除文件
- `rsync`:pid文件
- `mysql`:sock文件
+ 进程
+ 端口
5. `case`里的返回值不使用`xx && exit 0 || exit 1`,而是使用`RETVAL`变量接收返回值并统一返回(模仿rpcbind)
> https://github.com/orris27/orris/blob/master/linux/rsync/daemon.md

### 5-3. 检测站点文件是否被篡改
1. 如果将文件放到变量中的话,最好不要在程序里拼接路径.
+ `path=/tmp`:目录不要加最后的`/`,因为这样可以在拼接路径时显式指出`/`
+ `error_log="$path"/error.log`:这样就可以避免了,不要写`error_log=error.log`,这样没有意义
2. 如果管道前提示了错误信息就直接在管道前`2>/dev/null`,或者导出到我们的错误日志中
3. 比较文件是否有新增或减少,可以用`diff`,就是说将当前的文件名全放到文本文件中,然后比较就行了
> https://github.com/orris27/orris/blob/master/linux/shell/command.md


## 6. 关键
1. lock文件
2. 日志文件


### 注意
1. **如果使用`pkill`的话,自己的脚本文件绝对不要用对应的名字!!!如`pkill rsync`的话,自己的脚本不要取名叫做`rsync`!!!,甚至包含也不行!!**
+ 比如我脚本取名叫`rsync`,而我停止rsync服务端是用`pkil rsync`的,实际上就把包括自己脚本在内的所有包含`rsync`字符串的进程都kill了.而被kill之后就会显示Terminated.
