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
