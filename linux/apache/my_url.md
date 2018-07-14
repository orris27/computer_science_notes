用Apache httpd服务简单解析域名.

## 添加要用来作为域名的站点目录
```
# shell
cd /var/
sudo mkdir html/{www,blog} -p
sudo chown -R www.www html/{www,blog}
cd html/
sudo touch index.html
sudo vim index.html
cd blog/
sudo touch index.html
sudo vim index.html
```
## 指定域名和站点目录对

在apache/conf/extra/httpd-vhosts.conf下改成该代码
```
# apache/conf/extra/httpd-vhosts.conf
<VirtualHost *:80>
    ServerAdmin 993790240@qq.com
    DocumentRoot "/var/html/www"
    ServerName www.orris.com
    ServerAlias orris.com
    ErrorLog "logs/www-error_log"
    CustomLog "logs/www-access_log" common
</VirtualHost>

<VirtualHost *:80>
    ServerAdmin 993790240@qq.com
    DocumentRoot "/var/html/blog"
    ServerName blog.orris.com
    ErrorLog "logs/blog-error_log"
    CustomLog "logs/blog-access_log" common
</VirtualHost>
```
注意:创建/var/html/{www,blog}两个目录,其目录下都有index.html

## 配置主文件
1. 添加该vhosts配置文件
2. 设置权限
在apache/conf/httpd.conf下添加
```
# apache/conf/httpd.conf
Include conf/extra/httpd-mpm.conf

Include conf/extra/httpd-vhosts.conf

<Directory "/var/html">
    Options -Indexes +FollowSymLinks
    AllowOverride None
    Require all granted
</Directory>
```
## 检查语法
```
# shell
apachectl -t
```
## 平滑重启
```
# shell
sudo /usr/local/bin/apachectl graceful
```

## 在自己电脑上配置hosts文件解析域名
在/etc/hosts文件下添加
```
# /etc/hosts
47.96.148.130 www.orris.com blog.orris.com
```
hosts文件的语法很简单,后面的域名对应第一列的ip地址,域名用空格分开

## 测试
hosts文件修改后可以用下列方法测试是否成功
```
# shell
ping www.orris.com
ping blog.orris.com
```
## 使用
在浏览器上输入```www.orris.com```,```blog.orris.com```就可以了
