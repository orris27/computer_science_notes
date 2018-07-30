## 1. 安装
1. 编译安装
> https://github.com/orris27/orris/blob/master/linux/saltstack/configuration.md
## 2. 配置文件
1. 外网的haproxy
    + 详细介绍看
    > https://blog.csdn.net/u012758088/article/details/78643704
```
global
    log 127.0.0.1 local3 info
    uid 99
    gid 99
    daemon
    nbproc 1
    maxconn 100000
    pidfile /var/run/haproxy.pid
    chroot /usr/local/haproxy
defaults
    mode http
    option http-keep-alive
    timeout connect 5000ms
    timeout client 50000ms
    timeout server 50000ms

listen stats
    bind 0.0.0.0:8888
    mode http
    stats uri /haproxy-status
    stats auth haproxy:saltstack
    stats hide-version
    stats enable

frontend frontend_www_example_com
    bind 172.19.28.82:80
    mode http
    log global
    option httplog
    default_backend backend_www_example_com

backend backend_www_example.com
    option forwardfor header X-REAL-IP
    option httpchk HEAD / HTTP/1.0
    balance source
    server web-node1 172.19.28.82:8080 check inter 2000 rise 30 fall 15
    server web-node2 172.19.28.84:8080 check inter 2000 rise 30 fall 15
```
