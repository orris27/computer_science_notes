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

## 4. 实战
### 4-1. MySQL多实例脚本
> https://github.com/orris27/orris/blob/master/database/mysql/mysqld_multi.md
### 4-2. rsync服务端脚本(参考MySQL多实例脚本)
1. kill的时候,不适用`pkill`(如果使用,需要注意自己的脚本文件是否会被kill掉),而是使用pid文件
2. 使用pid文件需要判断pid文件是否存在
3. `stop`后不能马上`start`,因为可能会导致pid文件还没被删除
> https://github.com/orris27/orris/blob/master/linux/rsync/daemon.md

### 注意
1. **如果使用`pkill`的话,自己的脚本文件绝对不要用对应的名字!!!如`pkill rsync`的话,自己的脚本不要取名叫做`rsync`!!!,甚至包含也不行!!**
+ 比如我脚本取名叫`rsync`,而我停止rsync服务端是用`pkil rsync`的,实际上就把包括自己脚本在内的所有包含`rsync`字符串的进程都kill了.而被kill之后就会显示Terminated.
