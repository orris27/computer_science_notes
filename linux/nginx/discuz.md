## Discuz安装
### 1. 下载
```
git clone https://gitee.com/ComsenzDiscuz/DiscuzX.git
```

### 2. (解压)移动到对应的站点目录
2-1. 把里面的内容全都拉出来
2-2. upload目录里面的内容再往上拉一级

### 3. 创建数据库和用户
```
create database bbs default charset utf8;
grant all on bbs.* to 'bbs'@localhost identified by 'bbs';
flush privileges;
```

### 4. 配置好Nginx的参数,使得我们能够访问到刚才的目录,以及解析PHP
```
server {
    listen 80;
    server_name localhost;
    root html/bbs;
    index index.php index.html index.htm;

    location ~ .*\.(php|php5)?$ {
        fastcgi_pass   127.0.0.1:9000;
        fastcgi_index  index.php;
        include fastcgi.conf;
        include proxy.conf; # 自己配置的proxy优化参数
    }
}
```

### 5. 授权目录文件
这里我无论是该成nginx用户还是授权777都显示不可写,所以就没进行下去.其实后面的内容和hdwiki一样的.要修改rewrite的话,只要进入到对应的rewrite界面,然后会生成rewrite代码,复制到我们Nginx的配置文件里面,然后重启Nginx就好了
