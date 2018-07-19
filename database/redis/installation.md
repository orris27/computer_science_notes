## 安装步骤
```
wget http://download.redis.io/releases/redis-4.0.10.tar.gz
tar -zxf redis-4.0.10.tar.gz 
cd redis-4.0.10
less README.md 
sudo make MALLOC=jemalloc
sudo make PREFIX=/application/redis-4.0.10 install
sudo ln -s /application/redis-4.0.10/ /application/redis
```
