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
