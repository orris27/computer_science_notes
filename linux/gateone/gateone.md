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

