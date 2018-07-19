## 1. 错误提醒
如果执行失败,要在stderr中给出错误提醒,并且有必要的话要返回错误代码并退出程序
### 1-1. `&&`, `||`
```
cd $LOG_DIR || {
  echo "Cannot change to necessary directory." >&2
  exit 1
}
```


## 2. 每一句话都是有前提成立
比如只有正确cd进目录才能执行,只有文件存在才能执行语句
### 2-1. cd
```
cd /var/run/
```
### 2-2. `make`成功后才能执行`make install`
```
sudo make && sudo make install
```

