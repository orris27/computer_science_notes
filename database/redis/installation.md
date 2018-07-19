## 安装步骤
1. 编译安装(需要读`README.md`,和普通的有点差异)
```
wget http://download.redis.io/releases/redis-4.0.10.tar.gz
tar -zxf redis-4.0.10.tar.gz 
cd redis-4.0.10
less README.md 
sudo make MALLOC=jemalloc
sudo make PREFIX=/application/redis-4.0.10 install
sudo ln -s /application/redis-4.0.10/ /application/redis
```
2. 添加到环境变量
```
sudo vim /etc/profile
###
PATH=/application/redis/bin:$PATH
###
source /etc/profile
```
