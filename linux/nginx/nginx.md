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
    
### 2-1. upstream
1. 6和8是Web应用
2. 7和9是负载均衡器.nginx+keepalived
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







