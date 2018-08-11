## 1. 语法
### 1-1. 变量
1. 根据用户代理进行过滤
```
vim /etc/nginx/nginx.conf
##########################################################
server {
    set $block_user_agent 0;
    if ($http_user_agent ~ "Wget|ApacheBench") {
        set $block_user_agent 1;
    }
    if ($block_user_agent = 0) {
        return 403;
    }
}
##########################################################
nginx -t
nginx -s reload

ab -n 1 -c 1 192.168.56.10/
curl 192.168.56.10
```
## 2. 反向代理
对于请求的特定URL,通过`proxy_pass`发送给upstream模块,upstream根据调度算法在自己的服务池里选择服务器处理请求
1. URL=>server
    1. location判断域名+URI
2. server=>upstream
    + server里使用upstream的名字来指定
```
upstream blog_real_servers {
    server 10.0.0.6:80 weight=5;
    server 10.0.0.8:80 weight=10;
}
server {
    listen 80;
    server_name blog.etiantian.org;
    location / {
        proxy_pass http://blog_real_servers;
        include extra/proxy.conf;
    }
}
```
    
### 2-1. upstream
#### 2-1-1. 支持的代理方式
1. `proxy_pass`: 反向代理
2. `fastcgi_pass`: 和动态应用程序交互(Tomcat可以用proxy_pass)
3. `memcached_pass`: 和NoSQL做交互
#### 2-1-2. 位置
http标签内
```
http {
    upstream {
        ip_hash;
        server 10.0.0.10:80 weight=1 max_fails=2 fail_timeout=20s bakcup
    }
}
```
#### 2-1-2. 语法
1. `max_fails`:最大尝试失败次数.向下面curl2次
    + 京东1次,蓝讯10次.
    + 京东是为了用户体验
    + 蓝讯用户体验没京东大,但是给很多公司提供CDN,里面的服务器不能随意替换
2. `fail_timeout`: 睡眠时间为20秒(curl的间隔)
    + 常规业务2-3秒
3. `backup`: backup会在上面的服务都挂了就顶上去=>说明Nginx可以实现节点高可用
4. real server. 不写端口就是80
5. `weight`: 权重
6. 调度算法:默认权重轮询wrr
7. `down`:这个服务器不加入负载均衡集群里.也就是说我们不用注释它,而是直接加个down就行了.


### 2-2. 架构实例
1. 高可用集群
    + 7和9是负载均衡器
    + nginx+keepalived
2. 负载均衡集群
    + 6和8是Web应用
```
#########################################################
# 2个LBS
#########################################################

# Keepalived
#########################################################
# [root@lbs07 ~]# cat /etc/keepalived/keepalived.conf
# global_defs {
#     notification_email {
#         xxx@qq.com
#     }
#     notification_email_from Alexandre.Cassen@firewall.loc
#     smtp_server 10.0.0.1
#     smtp_connect_timeout 30
#     router_id LVS_7
# }
# 
# vrrp_instance VI_1 {
#     state MASTER
#     interface eth0
#     virtual_router_id 51
#     priority 100
#     advert_int 1
#     authentication {
#         auth_type PASS
#         auth_pass 1111
#     }
#     virtual_ipaddress {
#         10.0.0.10/24
#     }
# }

# [root@lbs09 ~]# cat /etc/keepalived/keepalived.conf
# global_defs {
#     notification_email {
#         xxx@qq.com
#     }
#     notification_email_from Alexandre.Cassen@firewall.loc
#     smtp_server 10.0.0.1
#     smtp_connect_timeout 30
#     router_id LVS_2
# }
# 
# vrrp_instance VI_1 {
#     state BACKUP
#     interface eth0
#     virtual_router_id 51
#     priority 50
#     advert_int 1
#     authentication {
#         auth_type PASS
#         auth_pass 1111
#     }
#     virtual_ipaddress {
#         10.0.0.10/24
#     }
# }
#########################################################




# Nginx
cd /application/nginx/conf && \
mkdir extra && \
cat > extra/upstream-1.conf<<EOF
upstream blog_real_servers {
    server 10.0.0.6:80 weight=5;
    server 10.0.0.8:80 weight=10;
}
server {
    listen 80;
    server_name blog.etiantian.org;
    location / {
        proxy_pass http://blog_real_servers;
    }
}
EOF


cd /application/nginx/conf && \
vim nginx.conf
###############################################
include extra/upstream-1.conf;
###############################################
nginx -t
nginx -s reload

#########################################################
# 所有主机都配置hosts文件
#########################################################
echo "10.0.0.10 blog.etiantian.org" >> /etc/hosts


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 浏览器访问blog.etiantian.org
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
```



## 3. 指令
### 3-1. upstream
见上面
### 3-2. location
处理请求的URI
#### 3-2-1. 语法
> [location官方文档](http://nginx.org/en/docs/http/ngx_http_core_module.html#location)
不同location有优先级,不是根据配置文件的先后顺序
1. `=`: 精确匹配
2. `~`: 区分大小写,正则
3. `~*`: 不区分大小写,正则
4. `^~`: 只匹配字符串,不匹配正则表达式<=>不写
5. `/`: 默认

### 3-3. proxy_pass
属于ngx_http_proxy_module模块,可以将请求发送到另一个server.
+ 代理层是Nginx,但是传过去的服务器可能也会是Nginx,Apache
#### 3-3-1. 参数
1. `proxy_set_header`: 让后端的服务器获得用户的IP.用户请求代理,代理去请求后端服务器.对于后端服务器来说,代理是客户.但设置了这个参数后,用户才是客户
    1. `proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;`:如果后端服务器的程序需要获取用户IP,从该Header头中获取
        + 保持用户的IP
        + `$remote_addr`:用户的IP
        + 如果修改日志格式,后台Web服务器日志会记录用户的IP,而不是记录代理的IP
            + 接收代理的服务器要修改日志格式才能够生效
                1. `Apache`:`LogFomat "\"${X-Forwarded-For}i\" %l %u ..." LogFormatName`
                2. `Nginx`:`log_format log_format_name 'xx "$http_x_forwarded_for"'`(不是remote_addr!!!)
                    + 同理对于接收代理请求的Nginx服务器的配置文件来说,$host就是`blog.orris.com`,而$http_x_forwarded_for就是用户的IP
                    + 这些变量名是固定的!!!不能自己胡乱修改!!!
                    + 在代理层是$proxy_add_x_forwarded_for,而在Web层是$http_x_forwarded_for.不一样!!
    2. `proxy_set_header Host $host`:传给的下一个比如说是Nginx的话,上面如果配置有多个虚拟主机,用该host区分域名
        + 保持用户的域名
        + 主要解决域名=>代理=>Nginx/Apache时域名的保持,否则发第一个
        + 后台Web服务器会区分www和blog等
        
2. `client_body_buffer_size`:可以先保存到本地一些数据大小,然后传给用户
3. `proxy_connect_timeout`: 代理与后端的服务器连接的超时时间
#### 3-3-2. 生产环境
```
cd /application/nginx/conf
vim extra/proxy.conf
# 不能cat多行输入,因为$符号会被解析为Shell变量
#######################################
proxy_redirect off;
proxy_set_header Host $host;
proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
proxy_connect_timeout 90;
proxy_send_timeout 90;
proxy_read_timeout 90;
proxy_buffer_size 4k;
proxy_buffers 4 32k;
proxy_busy_buffers_size 64k;
proxy_temp_file_write_size 64k;
#######################################

vim nginx.conf
#######################################
location xx {
    include extra/proxy.conf
}
#######################################
```


## 4. 动静分离(Nginx代理层=>Nginx后台服务器)
根据用户请求的URL判断访问动态服务器还是静态服务器
1. 规定static开头的URL访问静态服务器,而dynamic开头的URL访问动态服务器
2. location通过URL判断,用proxy_pass传给其他server(指upstream)
    + 如果是`location /static/`的话,那么URI为`/static`的话curl是找不到的,但浏览器可以找到,因为会自动添加末尾的`/`
3. 为不同的upstream模块static_pools/dynamic_pools分配相应的静态服务器和动态服务器
4. 在静态服务器上开放可以响应static开头URL请求的功能,而使动态服务器能响应dynamic开头URL的请求
    + URI要匹配2次.代理层和Web层均要匹配1次
```
#####################################################################################
# static servers 10.0.0.6
#####################################################################################
cd /application/nginx/html/www
mkdir static
echo "www-10.0.0.6-static" > static/index.html



#####################################################################################
# dynamic servers 10.0.0.8
#####################################################################################
cd /application/nginx/html/www
mkdir dynamic
echo "www-10.0.0.8-dynamic" > dynamic/index.html



#####################################################################################
# LBS
#####################################################################################

vim /application/nginx/conf/nginx.conf
#######################################################################
upstream static_pools {
    server 10.0.0.6:80 weight=5 max_fails=2 fail_timeout=20s;
}
upstream dynamic_pools {
    server 10.0.0.8:80 weight=5 max_fails=2 fail_timeout=20s;
}

server {
    location /static/ {
        proxy_pass http://static_pools;
        include extra/proxy.conf;
    }
    location /dynamic/ {
        proxy_pass http://dynamic_pools;
        include extra/proxy.conf;
    }
}
#######################################################################
nginx -t
nginx -s reload


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 浏览器分别访问www.orris.com/static(10.0.0.10绑定了VIP和www.orris.com)
# 浏览器分别访问www.orris.com/dynamic
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


```
## 5. PC端和移动端分离(Nginx代理层=>Nginx后台服务器)
根据用户请求的URL判断访问PC端服务器还是移动端服务器
1. location里面通过用户代理(变量)来判断,用proxy_pass传给其他server(指upstream)
    + 如果包含iphone或android,就去找移动端服务器
    + 默认发给PC端服务器比较好
2. 为不同的upstream模块static_pools/dynamic_pools分配相应的PC端服务器和移动端服务器
```
# 以下只是示例,非生产环境的代码
location / {
    if ($http_user_agent ~* 'android') {
        proxy_pass http://android_pools;
    }
    if ($http_user_agent ~* 'iphone') {
        proxy_pass http://android_pools;
    }
}
```


## 6. Nginx反向代理层查看后台服务器状况
Nginx尽管有健康检查,但默认不能像LVS一样查看后台服务器的状态.思路如下
+ `nginx_upstream_check_module`模块
+ 自己写脚本,对每个后台服务器做健康检查


## 7. 静态缓存
proxy_cache


## 8. TCP代理
> [官方TCP代理文档](https://nginx.org/en/docs/stream/ngx_stream_core_module.html),是4层!
### 8-1. 代理ssh
> 如果原来已经安装了Nginx,重新configure+make+make install的话,不相干的文件和配置文件都不会发生改变!!!(实测)
```
mkdir ~/tools
cd ~/tools/nginx-1.14.0/
sudo ./configure --user=nginx --group=nginx --prefix=/application/nginx-1.14.0 --with-http_stub_status_module --with-http_ssl_module --with-stream
make && make install



vim /application/nginx/conf/nginx.conf
######################################################################
stream {
    upstream backend_ssh {
        hash $remote_addr consistent;
        server 10.0.0.8:22  max_fails=3 fail_timeout=30s;
    }

    server {
        listen 12345;
        proxy_connect_timeout 1s;
        proxy_timeout 3s;
        proxy_pass backend_ssh;
    }
}

######################################################################


netstat -lntup | grep 12345
#------------------------------------------------------------------------
# tcp        0      0 0.0.0.0:12345           0.0.0.0:*               LISTEN      84497/nginx: master 
#------------------------------------------------------------------------
# 然后我们就可以像对待10.0.0.8:22一样对待我们的10.0.0.7:12345了!!!
ssh root@10.0.0.7 -p 12345

```


## 9. 让Nginx用多个IP地址去连接后台服务器来解决端口不够的问题

2. 知识点测试
```
#########################################################################
# 10.0.0.8
#########################################################################

vim /application/nginx/conf/nginx.conf
##############################################################################
upstream backend {
    server 10.0.0.8:80 weight=10;
}
http {
    server {
        location / {
            proxy_pass http://backend;
        }
    }
}
##############################################################################
nginx -s reload

curl 10.0.0.7
curl 10.0.0.7
curl 10.0.0.7



###########################################################################
# 10.0.0.{7,8}
###########################################################################

netstat -nat | grep 'TIME_WAIT'
#------------------------------------------------------------------------------
[root@lbs07 ~]# netstat -nat | grep 'TIME_WAIT'
tcp        0      0 10.0.0.7:39166          10.0.0.7:80             TIME_WAIT 
[root@master-8 ~]# netstat -nat | grep 'TIME_WAIT'
tcp        0      0 10.0.0.8:80             10.0.0.7:34844          TIME_WAIT  
#------------------------------------------------------------------------------
# 10.0.0.7:39166=>10.0.0.7:80
# 说明如果请求代理服务器,且不是长连接时,是后台服务器主动关闭TCP连接=>
# TIME_WAIT=1m也得到测试

```

```
ifconfig eth0:0 10.0.0.100/24 up
ifconfig eth0:1 10.0.0.101/24 up
ifconfig eth0:2 10.0.0.102/24 up
ifconfig eth0:3 10.0.0.103/24 up

vim /application/nginx/conf/nginx.conf
##############################################################################




##############################################################################

```
