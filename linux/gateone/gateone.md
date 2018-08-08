## 1. 安装
> [官方网站](https://github.com/liftoff/GateOne)+[官方文档](http://liftoff.github.io/GateOne/)
```
git clone https://github.com/liftoff/GateOne.git

cd GateOne/
vim INSTALL.txt 
yum install -y python-pip
pip install stdeb
pip install --upgrade distribute --user
python setup.py  install

pip install tornado==4.5.3 # GateOne只支持4.5.3版的tornado

sudo gateone

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 注意一定是HTTPS协议!浏览器访问https://192.168.56.10:443
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
```

## 2. 使用
### 2-1. ssh终端
1. 输入IP地址,如`192.168.56.10`
2. 输入相关信息
3. 和SSH连接一样自由玩耍把~

### 2-2. 使用Log View将golog文件以video的形式播放
> [logviewer官方文档](http://liftoff.github.io/GateOne/Developer/logviewer.html?highlight=golog)
#### 2-2-1. options
1. `--flat`: Display the log line-by-line in a syslog-like format.
2. `--raw`:Display control characters and escape sequences when viewing.
#### 2-2-2. 使用
1. 查看帮助
```
/usr/lib/python2.7/site-packages/gateone-1.2.0-py2.7.egg/gateone/applications/terminal/logviewer.py --help
```
2. 文本方式查看
```
/usr/lib/python2.7/site-packages/gateone-1.2.0-py2.7.egg/gateone/applications/terminal/logviewer.py --flat /var/lib/gateone/users/ANONYMOUS/logs/20180808122938602972-192.168.56.1.golog
```
3. video形式播放
```
/usr/lib/python2.7/site-packages/gateone-1.2.0-py2.7.egg/gateone/applications/terminal/logviewer.py  /var/lib/gateone/users/ANONYMOUS/logs/20180808122938602972-192.168.56.1.golog

```
