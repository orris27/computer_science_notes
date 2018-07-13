### 1. 隐藏版本
#### 出现Nginx的版本情况
1. 502的时候下面会出现Nginx版本号
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
