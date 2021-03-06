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
```/serv
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
worker_cpu_affinity 0001 0010 0100 1000;shixiao
```
##### 3. 八核CPU
```
# worker_processes  8;
worker_cpu_affinity 10000000 01000000 00100000 00010000 00001000 00000100 00000010 00000001;
```

### 6. 设置事件处理模型(epoll,select..)
#### 解决方法
```
events {
    use epoll;
    # ....
}
```

### 7. 设置单个进程允许的最大连接数
#### 解决方法
```
events {
    worker_connections  20480;
    # ...
}
```

### 8. 设置单个进程最大打开文件数
#### 解决方法
```
worker_rlimit_nofile 65535; # main 标签里
```


### 9. 设置连接超时时间
php希望短连接,Java希望长连接
#### 解决方法caixing
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

### 11. 优化服务器名字的hash表大小(可选)
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

### 12. 用户上传文件的大小限制
php默认是2m,一般10m就已经很大
#### 解决方法deny all
```
# context: http, server, location
client_max_body_size 10m;
```
### 13. fastcgi调优
#### 解决方法
###### 注意
如果我们在Nginx的conf下创建了extra目录,那么在extra下的conf的相对路径仍然是conf而言,而不是extra
##### 注释版
```
# http处
fastcgi_cache_path /usr/local/nginx/fastcgi_cache levels=1:2 keys_zone=orris_cache:10m inactive=5m; # 为FastCGI缓存指定一个文件路径、目录结构等级、关键字区域存储时间和非活动删除时间。
fastcgi_connect_timeout 300; # Nginx和PHP连接的超时间
fastcgi_send_timeout 300;
fastcgi_read_timeout 300;
fastcgi_buffer_size 64k;
fastcgi_buffers 4 64k; 
fastcgi_busy_buffers_size 128k; # 建议为fastcgi_buffers的两倍
fastcgi_temp_file_write_size 128k; # 在写入fastcgi_temp_path时使用多大的数据块,默认值是fastcgi_buffers的两倍,设置太小而负载大的话就会502
fastcgi_cache orris_cache; # 设置fastcgi缓存,并指定名称.可以有效降低CPU负载,并且防止502,但也有其他问题.需要设置fastcgi_cache_path才行
fastcgi_cache_valid 200 302 1h; # 用来指定应答代码的缓存时间,这行表示200和302应答缓存为1个小时
fastcgi_cache_valid 301 1d;
fastcgi_cache_valid any 1m; # 其他应答设置为1分钟
fastcgi_cache_min_uses 1; # 缓存在fastcgi_cache_push指令inactive参数值时间内的最少使用次数
```
##### 无注释
```
# http处
fastcgi_cache_path /application/nginx/fastcgi_cache levels=1:2 keys_zone=orris_cache:10m inactive=5m;
fastcgi_cache orris_cache;
fastcgi_connect_timeout 300;
fastcgi_send_timeout 300;
fastcgi_read_timeout 300;
fastcgi_buffer_size 64k;
fastcgi_buffers 4 64k;
fastcgi_busy_buffers_size 128k;
fastcgi_temp_file_write_size 128k;
fastcgi_cache_valid 200 302 1h;
fastcgi_cache_valid 301 1d;
fastcgi_cache_valid any 1m;
fastcgi_cache_min_uses 1;
fastcgi_cache_key http://$host$request_uri;

```

### 14. 更改源码隐藏响应头和错误代码里的Nginx服务器信息
#### 思路
1. 修改源码
2. 备份`conf`这些配置文件的目录
3. 通过`-V`或`Makefile`或`configure`文件来获取原来的编译参数
4. 从`./configure`开始一直到`make install`执行一遍,但是make install前停止nginx才行(`sudo pkill nginx`或`sudo nginx -s stop`)
5. 启动服务器就ok了
#### 解决方法
1. 备份以及修改源码
```
# curl -I 127.0.0.1
# sudo find /  -name 'ngx_http_header_fil*' # 这里会得到.o和.c文件,他们都是我们下载时解压得到的目录下的c文件,以及make出来的目标文件

sudo cp ~/tools/nginx-1.14.0/src/http/ngx_http_header_filter_module.c ~/tools/nginx-1.14.0/src/http/ngx_http_header_filter_module.c.bak
sudo vim ~/tools/nginx-1.14.0/src/http/ngx_http_header_filter_module.c
#--vim starts-- 48 行开始改成下面内容
static u_char ngx_http_server_string[] = "Server: BWS" CRLF;
static u_char ngx_http_server_full_string[] = "Server: BWS" CRLF;
static u_char ngx_http_server_build_string[] = "Server: " NGINX_VER_BUILD CRLF;
#--vim ends--

cd ~/tools/nginx-1.14.0/src/http/
cp ngx_http_special_response.c ngx_http_special_response.c.bak
vim ngx_http_special_response.c
#--vim starts-- 36行改成
"<hr><center>BWS</center>" CRLF
#--vim ends--

```
2. 备份原来服务器的conf参数(重新编译原来的参数也不会改变)
```
cd /application/nginx/
sudo tar -zcf conf.tar.gz conf
```
3. 开始configure->pkill->make install
```
sudo nginx -V

cd ~/tools/nginx-1.14.0
sudo ./configure --user=nginx --group=nginx --prefix=/application/nginx-1.14.0 --with-http_stub_status_module --with-http_ssl_module
sudo make 
sudo nginx -s stop
sudo make install
sudo nginx
```

4. 检测
```
curl -I 127.0.0.1
curl 127.0.0.1/12312312
```
### 15. 配置Nginx的gzip压缩功能
#### 分析
1. 如果使用压缩的话,那么会消耗服务器资源,但是传输速度块
2. 一般压缩:程序文件(html,css,js,xml)
3. 一般不压缩:小文件,图片,视频,flash等
#### 解决方法
##### 有注释版
```
# http,..
gzip on;
gzip_min_length 1k; # 小于1k的文件就不压缩了
gzip_buffers 4 16k;
# gzip_http_version 1.0; # 一般选默认就好了
gzip_comp_level 3; # 压缩比越大=>压缩越彻底,传输速度最快,处理最慢(1-9)
gzip_types text/html text/css application/javascript text/xml; # 压缩的对象.具体是什么可以参考conf/mime.types
gzip_vary on; # 允许前端的服务器缓存经过gzip压缩的文件
```
##### 无注释
```
# http
gzip on;
gzip_min_length 1k;
gzip_buffers 4 16k;
# gzip_http_version 1.0; 
gzip_comp_level 3; 
gzip_types text/css application/javascript text/xml;
gzip_vary on; 
```
#### 检测方法
1. 安装yslow插件
2. F12后点击yslow,在componets处点击某个可能被压缩的,看gzip处的情况


### 16. 缓存的过期实际那
#### 缓存依据
1. 不希望被缓存的
+ 广告图片
+ 更新频繁的文件(google的logo)
+ 网站流量统计的工具
2. 缓存标准(淘宝为例)
+ 普通的js(10年)
+ 频繁更新的js(1天)
+ 流量统计的js(没有缓存)
+ 普通图片(10年)
#### 查看某个网站的缓存
YSlow的components里可以看到
#### 配置
可以通过配置不同虚拟主机,不同的目录来设置同一类型文件的不同过期时间
```
# server标签下
location ~ .*\.(gif|jpg|jpeg|png|bmp|swf)?$ {
    root html;
    expires 3650d;
}
location ~ .*\.(js|css)?$ {
    root html;
    expires 30d;
}
location ~ (robots.txt) {
    log_not_found off;
    expires 7d;
    break;
}
```

### 17. 防爬虫
#### 解决方法
1. `robots.txt`协议,不过这是非强制性的
2. Nginx服务器禁止某些用户代理访问,强制性
```
# server标签下
# 这里正则匹配,防止火狐,ie等访问.可以自己设置
if ($http_user_agent ~* "Firefox|MSIE") {
    return 403;
}
# 测试:直接用火狐和Chrome来访问看看就行了
```

### 18. 日志优化
1. Nginx默认一直写日志,直到磁盘满
2. 不要用vim打开几个G的文件,系统会爆炸
3. 定时打包日志
4. 不需要的记录不用写到日志中,如图片,js,css的访问和健康检查(比如Nginx检查端口,这个只要负载均衡器后面的服务器配置就好了)
5. 设置访问日志的权限为root
#### 解决方法
1. 定时打包日志
```
# conf/httpd.conf
error_log logs/error.log error;

sudo vim /server/scripts/cut_nginx_log.sh
##########
#! /bin/sh

cd /application/nginx/logs/ && \
/bin/mv www-access.log www-access-$(date +%F -d -1day).log
/application/nginx/sbin/nginx -s reload
###########


sudo crontab -e
###########
# cut nginx log
00 00 * * * /bin/sh /server/scripts/cut_nginx_log.sh > /dev/null 2>&1
###########
```
2. 不记录不需要记录的日志
```
location ~ .*\.(js|jpg|JPG|jpeg|JPEG|css|bmp|gif|GIF|png)$ {
    access_log off;
}
```
3. 设置访问日志的权限为root(尽管worker进程的用户是nginx外部用户,但写日志用的是master进程的用户,就是root,所以我们改成root)
```
sudo chown -R root.root /application/nginx/logs
sudo chmod 700 /application/nginx/logs
```

### 19. 站点目录和文件的权限设置
#### 思路
##### 最安全的策略:
1. 所有站点目录和文件的用户和组都是root
2. 所有目录的权限是755
3. 所有文件权限为644
##### 动静分离
动态Web服务器,静态服务器(图片等),上传服务器集群等都分开来,根据自己情况设置
##### 注意
外部用户不要是root

### 20. 限制文件或程序访问(根据自己需要进行设置)
#### 思路
+ 扩展名
#### 解决方法
##### 1.某个目录下的程序
建议:这一类禁止的location,最好写在其他location前面
```
# images目录下,如果访问.php文件,就会返回403
location ~ ^/images/.*\.(php|php5)$ {
    deny all;
}
```
##### 2. 多个目录下的程序
```
# images和static目录下,都禁止访问.php文件
location ~ ^/(images|static)/.*\.(php|php5)$ {
    deny all;
}
```
##### 3. 某个目录不能访问
```
location ~ ^/images {
    deny all;
}
```

### 21. 限制IP访问
一般在防火墙里设置,不过Nginx也可以设置
#### 解决方法
```
# 注意location / 只针对严格匹配/的情况
location / {
    allow 172.19.28.83;
    deny 172.19.28.82;
    deny 172.19.28.0/24;
    # deny all; # deny all最好不要和黑名单/白名单一起用,否则全部都是403
}

```

### 22. 错误提示优雅显示
#### 方法
```
error_page  404              /404.html;

# redirect server error pages to the static page /50x.html
#
error_page   500 502 503 504  /50x.html;
location = /50x.html {
    root   html;
}
```

### 23. 使用tmpfs文件系统替代频繁访问的目录
tmpfs文件系统是基于内存的,如果我们发现某个目录的I/O很高的话,就可以考虑把这个目录的文件系统弄成tmpfs
#### 思路
1. 将目录A的内容全移到另一个地方(备份)
2. 将目录A挂载到tmpfs的文件系统上
3. 定时清理文件
#### 解决方法
1. 默认情况下,`/dev/shm`就是tmpfs的文件系统,如果对该文件进行读写,那么实际上内容写在了虚拟内存中
2. 要使用tmpfs的话,需要在内核配置中,启用`Virtual memory file system support`
3. 要防止tmpfs使用了全部的VM,需要限制其大小
4. 使用mount的话,还可以通过设置mount的`noexec`,`nosuid`来限制用户行为
```
sudo mkdir -p /opt/tmp
sudo mv /tmp/* /opt/tmp
sudo mount -t tmpfs -o size=16m tmpfs /tmp # 一般size设置为2G-8G,这里16m只是测试
df -h
sudo vim /etc/fstab
#####
tmpfs  /tmp  tmpfs size=2048m 0 0
#####
```

### 24. proxy调优
#### 解决方法
1. 将参数放在`proxy.conf`下
```
# sudo vim /application/nginx/conf/proxy.conf
###

proxy_redirect off;
proxy_set_header Host $host;
proxy_set_header X-Real-IP $remote_addr;
proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
client_max_body_size 50m;
client_body_buffer_size 256k;
proxy_connect_timeout 30;
proxy_send_timeout 30;
proxy_read_timeout 60;

proxy_buffer_size 4k;
proxy_buffers 4 32k;
proxy_busy_buffers_size 64k;
proxy_temp_file_write_size 64k;
proxy_next_upstream error timeout invalid_header http_500 http_503 http_504;
proxy_max_temp_file_size 128m;
proxy_store on;
proxy_store_access user:rw group:rw all:r;
#proxy_temp_path /dev/shm/nginx;
#proxy_temp_path /data2/nginx_cache;

###
```
2. 在解析php的地方包含进来
```
sudo vim /application/nginx/conf/nginx.conf
####
location ~ .*\.(php|php5)?$ {
    fastcgi_pass   127.0.0.1:9000;
    fastcgi_index  index.php;
    include fastcgi.conf;
    include proxy.conf;
}
####
```

### 25. 防止其他网站盗用我们的资源
#### 原因
其他网站偷用自己的资源,如用`src`,`href`等
#### 思路
1. 过滤(通过请求信息来判断,黑名单/白名单)
+ 根据IP封杀(IP访问特别多,日志里面可以看到)
+ 防盗链
    + referer:过滤(referer本身是HTTP的一个字段,如果只是用浏览器打开的话,是没有referer字段的)
    + cookie
2. 共赢
+ 加上自己的广告,爱用不用,反正自己也有广告宣传
#### 预防措施
1. 运维人员多关注网站流量情况
2. 告警机制
#### 解决方法
```
# 对于five.org来说,要想访问jpg等资源,只能是five.org的域名,否则就返回给你error.jpg
location ~* \.(jpg|gif|png|swf|flvv|wma|wmv|asf|mp3|mmf|zip|rar)$ {
    valid_referers none blocked *.five.org five.org;
    if ($invalid_referer) {
        rewrite ^/ http://www.five.cn/error.jpg;
    }
}
```
