## 1. 配置文件里添加参数
```
location / {
    auth_basic "orris";
    auth_basic_user_file htpasswd; # 如果这个配置文件是在/application/nginx/conf/下的,那么这个htpasswd文件就是/application/nginx/conf/htpasswd
}
```

## 2. 注册用户名和密码
需要安装Apache,然后它提供的htpasswd可以帮助我们注册.当然也可以用其他方式
```
sudo htpasswd -c /application/nginx/conf/htpasswd orris # 交互
sudo htpasswd -cb /application/nginx/conf/htpasswd orris 123456 # 非交互
```
