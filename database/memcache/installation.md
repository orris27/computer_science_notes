## 1. 客户端
### PHP
#### 安装
> https://github.com/orris27/orris/blob/master/php/optimization.md
#### 检验
1. 是否有`so`文件
```
ll /application/php/lib/php/extensions/no-debug-zts-20131226/
```
2. 是否配置了memcache
+ `extension_dir`目录下是否有so文件
+ 如果配置session的话,`php.ini`中的`session.save_handler`和`session.save_path`是否调整好
- `session.save_handler = memcache`
- `session.save_path = "tcp://10.0.0.7:11211"`
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
