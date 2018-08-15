## 1. 安装及配置
### 1. 编译安装(需要读`README.md`,和普通的有点差异)
```
wget http://download.redis.io/releases/redis-4.0.10.tar.gz
tar -zxf redis-4.0.10.tar.gz 
cd redis-4.0.10
less README.md 
sudo make MALLOC=jemalloc
sudo make PREFIX=/application/redis-4.0.10 install
sudo ln -s /application/redis-4.0.10/ /application/redis
echo 'PATH=/application/redis/bin:$PATH' >> /etc/profile
source /etc/profile
```
### 3. 配置
#### 3-1. 获取配置文件
```
sudo mkdir /application/redis/conf
sudo cp ~/tools/redis-4.0.10/redis.conf /application/redis/conf/
```
#### 3-2. 更改配置文件
> https://github.com/orris27/orris/blob/master/database/redis/redis_conf.md


### 4. 启动服务
#### 4-1. 直接启动服务
```
sudo redis-server /application/redis/conf/redis.conf &
```
#### 4-2. 提示错误`WARNING overcommit_memory is set to 0! Background save may fail under low memory condition. To fix this issue add 'vm.overcommit_memory = 1' to /etc/sysctl.conf and then reboot or run the command 'sysctl vm.overcommit_memory=1' for this to take effect.`,这说明内存不够的时候要执行这些命令
#### 4-3. 临时/永久解决问题
1. 临时
```
sudo sysctl vm.overcommit_memory=1
```
2. 永久
```
sudo vim /etc/sysctl.conf
###
vm.overcommit_memory = 1
###
```
#### 4-4. 提示错误`WARNING you have Transparent Huge Pages (THP) support enabled in your kernel. This will create latency and memory usage issues with Redis. To fix this issue run the command 'echo never > /sys/kernel/mm/transparent_hugepage/enabled' as root, and add it to your /etc/rc.local in order to retain the setting after a reboot. Redis must be restarted after THP is disabled.`
#### 4-5. 临时/永久解决问题
1. 临时
```
echo never > /sys/kernel/mm/transparent_hugepage/enabled
```
2. 永久
```
vim /etc/rc.local 
###
echo never > /sys/kernel/mm/transparent_hugepage/enabled
###
```
#### 4-6. 重启服务
```
sudo redis-cli shutdown
sudo redis-server /application/redis/conf/redis.conf &
```

## 2. 安全
> https://github.com/orris27/orris/blob/master/linux/security/security.md
1. 利用防火墙防止6379端口暴露在外网
```
iptables -t filter -A INPUT -p tcp --dport 6379 -j DROP
```
2. 设置redis密码
```
vim /application/redis/conf/redis.conf
#####################################
requirepass my_password
#####################################
redis-cli -h 172.19.28.82 shutdown
redis-server /application/redis/conf/redis.conf &
redis-cli -h 172.19.28.82  # 能进去,但访问受限
redis-cli -h 172.19.28.82 -a my_password
```
