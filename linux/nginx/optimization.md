### 1. 隐藏版本
#### 出现Nginx的版本情况
1. 502等错误的时候下面会出现Nginx版本号
2. `curl -I`的时候会出现Nginx版本
#### 解决方法
在http标签内加入`server_tokens off`

### 2. 解决恶意域名绑定
#### 解决方法
在第一处server标签前加一个server标签(也就是说下面的server标签必须是第一个server标签)处加上禁止所有成员访问,这样就阻止了ip访问
```
server {
    listen 80;
    location / {
        deny all;
    }
}
```

### 3. 更改Nginx的默认用户
#### Nginx的默认用户
默认情况是nobody
#### 解决方法
1. 配置文件中修改用户名和组
2. 在编译的时候指定用户名和组
#### 检查方法
查看进程的依赖用户(master process还是root,而worker process则是我们新的用户,如nginx)
```
ps -aux | grep nginx
```

### 4. 配置Nginx的进程个数
#### 平衡点
1. 并发多=>多进程
2. 服务器能力=>不能太多进程
#### 结论
worker_process的个数=CPU的核数
#### 解决方法
```
grep 'physical id' /proc/cpuinfo # 每一行表示一个CPU
sudo vim /application/nginx/conf/nginx.conf
#--vim starts--
worker_processes  xx; # 改成cpu核数就行了,因为我的服务器为1核,所以还是1
#--vim ends--
```

### 5. 把不同的进程分给不同的CPU
如果不设置CPU的亲和度的话,尽管我们设置多个进程,那么这些进程还是不一定会刚好分配给每个CPU核.有可能会所有worker进程都分配到CPU的一个核上,这样就会导致服务器CPU受压不均
#### 解决方法
在Nginx的main标签里设置CPU的亲和度,然后优雅重启
##### 1. 双核CPU
###### 1-1. 双核CPU开2个进程
```
# worker_processes  2;
worker_cpu_affinity 01 10;
```
###### 1-2. 双核CPU开8个进程
```
# worker_processes  8;
worker_cpu_affinity 01 10 01 10 01 10 01 10;
```
##### 2. 四核CPU
```
# worker_processes     4;
worker_cpu_affinity 0001 0010 0100 1000;
```
##### 3. 八核CPU
```
# worker_processes  8;
worker_cpu_affinity 10000000 01000000 00100000 00010000 00001000 00000100 00000010 00000001;
```

### 6. 设置事件处理模型(epoll,select..)
```
events {
    use epoll;
    # ....
}
```

### 7. 设置单个进程允许的最大连接数
```
events {
    worker_connections  20480;
    # ...
}
```

### 8. 设置单个进程最大打开文件数
```
worker_rlimit_nofile 65535; # main 标签里
```


### 9. 设置连接超时时间
php希望短连接,Java希望长连接
#### 解决方法
```
# context: http, server, location
keepalive_timeout 60;
tcp_nodelay on;
client_header_timeout 15;
client_body_timeout 15;
send_timeout 15;
```

### 10. 开启高效文件传输模式(可选)
#### 解决方法
```
tcp_nopush on;
tcp_nodelay on; # 前两个是为了防止网络阻塞
sendfile on; # http,server,location都可以
```

### 11优化服务器名字的hash表大小(可选)
如果在`server_name`处填写太多域名(如:`*.orris.com`这样的通配符),那么就会变慢(如:泛解析)
#### 解决方法
1. 尽量使用确切名字
2. 如果定义了大量名字,就需要设置`server_names_hash_max_size`和`server_names_hash_bucket_size`的值,比如说设置为默认值的一倍
```
http {
    server_names_hash_max_size 1024; # 如果默认值为512kb的话
    server_names_hash_bucket_size 64; # 如果默认值为32的话
}
```
