## 1. 客户端
### PHP
#### 安装
> https://github.com/orris27/orris/blob/master/php/optimization.md (1-3)
#### 检验
> https://github.com/orris27/orris/blob/master/php/optimization.md (10)
## 2. 服务端
```
sudo yum install libevent-devel -y
wget http://memcached.org/latest
tar zxf memcached-1.5.9.tar.gz 
cd memcached-1.5.9
sudo ./configure
sudo make && sudo make install
echo $?
which memcached
```
