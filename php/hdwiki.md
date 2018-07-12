## 在LNMP的基础上安装HDwiki来玩玩
1. 下载并调整HDwiki目录  
下载并解压hdwiki,将hdwiki的内容放在html/bbs下
```
# 在nginx/html目录下
sudo wget http://kaiyuan.hudong.com/download.php?n=HDWiki-v6.0UTF8-20170209.zip
# 可以考虑先把当前内容存放到新建的某个目录下
sudo unzip download.php\?n\=HDWiki-v6.0UTF8-20170209.zip 
sudo mv hdwiki/* bbs/
```
2. 配置nginx,使得nginx能够访问到html/bbs,并且可以执行php引擎
```
# nginx配置文件下:
server {
    listen 80;
    server_name blog.orris.com;
    root html/bbs;
    index index.php index.html index.htm;

    location ~ .*\.(php|php5)?$ {
        fastcgi_pass   127.0.0.1:9000;
        fastcgi_index  index.php;
        include fastcgi.conf;
    }
}
####
sudo nginx -t 
sudo ningx -s reload
```
3. 登录到页面,发现是在install界面,并且下面的内容是不可写的  
=>给html/bbs授权,让我们的nginx用户管理它
```
sudo chown -R nginx.nginx bbs/
```
4. 发现要有数据库,所以我们建一个专门为HDwiki服务的数据库,以及对应的用户
```
create database wiki default character set utf8;
grant all on wiki.* to wiki@'localhost' identified by 'wiki';
flush privileges;
```
5. 前缀最好换一个不是`wiki_`的,这样可以防止攻击
6. 一路安装就ok了
