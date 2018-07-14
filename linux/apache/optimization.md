### 1. 禁止对外提供目录
#### 解决方法
```
# httpd.conf
Options -Indexes +FollowSymLinks
```

### 2. 阻止Apache在-t/启动时的报错(非优化)
#### 解决方法
```
# httpd.conf
ServerName 127.0.0.1:80
```
### 3. 隐藏版本
#### 解决方法
1. 修改服务器版本:在`~/tools/httpd-2.4.33/include/ap_release.h`中修改宏定义
2. 修改运行后的操作系统:在`~/tools/httpd-2.4.33/os/unix/os.h`中修改宏定义
##### ap_release.h
```
#define AP_SERVER_BASEVENDOR "Apache Software Foundation"
#define AP_SERVER_BASEPROJECT "Apache HTTP Server"
#define AP_SERVER_BASEPRODUCT "Apache"

#define AP_SERVER_MAJORVERSION_NUMBER 2
#define AP_SERVER_MINORVERSION_NUMBER 4
#define AP_SERVER_PATCHLEVEL_NUMBER   33
#define AP_SERVER_DEVBUILD_BOOLEAN    0
```
##### os.h
```
#ifndef PLATFORM
#define PLATFORM "Unix"
#endif
```
### 4. 在响应头中隐藏版本号
1. 修改`apache/conf/extra/httpd-default.conf`下的参数
```
# modified into
ServerTokens Prod
ServerSignature Off
```
2. 在主配置文件中include该配置文件
```
Include conf/extra/httpd-default.conf
```

### 5. gzip压缩
#### gzip安装
<1> 编译时加入参数`--enable-deflate`
<2> `mod_deflate` DSO安装
```
cd ~/tools/httpd-2.4.33/modules/filters/
/application/apache/bin/apxs -c -i -a mod_deflate.c
ll /application/apache/modules/mod_deflate.so
```
#### 配置
1. 在`httpd.conf`中load该模块
```
sudo vim conf/httpd.conf
###
LoadModule deflate_module modules/mod_deflate.so
# 取消注释
###
```
2. 将模块添加到Apache的配置文件中
```
sudo vim conf/httpd.conf 
######
<IfModule mod_deflate.c>
	DeflateCompressionLevel 9
	SetOutputFilter DEFLATE
	AddOutputFilterByType DEFLATE text/html text/plain text/xml
	AddOutputFilterByType DEFLATE application/javascript
	AddOutputFilterByType DEFLATE text/css
</IfModule>
######
```
3. 平滑重启Apachegai
```
sudo apachectl -t
sudo apachectl graceful
```

### 6. 缓存过期时间设置
#### expire安装
<1> 编译时加入参数`--enable-expires`
<2> `mod_deflate` DSO安装
```
cd ~/tools/httpd-2.4.33/modules/metadata/
/application/apache/bin/apxs -c -i -a mod_expires.c
ll /application/apache/modules/mod_expires.so
```
#### 配置
1. 在`httpd.conf`中load该模块
```
sudo vim conf/httpd.conf
###
LoadModule expires_module modules/mod_expires.so # 取消注释
###
```
2. 将模块添加到Apache的配置文件中
```
sudo vim conf/httpd.conf # 安装到vhosts里或者主配置文件都可以
######
ExpiresActive on
ExpiresDefault "access plus 12 month"
ExpiresByType text/html "access plus 12 months"
ExpiresByType text/css "access plus 12 months"
ExpiresByType image/gif "access plus 12 months"
ExpiresByType image/jpeg "access plus 12 months"
ExpiresByType image/jpg "access plus 12 months"
ExpiresByType image/png "access plus 12 months"
ExpiresByType application/x-shockwave-flash "access plus 12 months"
ExpiresByType application/x-javascript "access plus 12 months"
ExpiresByType video/x-flv "access plus 12 months"
######
```
3. 平滑重启Apachegai
```
sudo apachectl -t
sudo apachectl graceful
```

### 7. 日志优化
1. Nginx默认一直写日志,直到磁盘满
2. 不要用vim打开几个G的文件,系统会爆炸
3. 定时打包日志
4. 不需要的记录不用写到日志中,如图片,js,css的访问和健康检查(比如Nginx检查端口,这个只要负载均衡器后面的服务器配置就好了)
5. 设置访问日志的权限为root
#### 解决方法
1. 定时打包日志(参见思维导图)
2. 不记录不需要的日志
```
# httpd.conf
sudo vim /application/apache/conf/httpd.conf
#### 设置环境变量IMAG
<FilesMatch "\.(css|js|gif|jpg|ico|swf|png)">
    SetEnv IMAG 1
</FilesMatch>
####

sudo vim /application/apache/conf/extra/httpd-vhosts.conf
#### 在对应记录日志(CustomLog)的地方,如主配置文件/虚拟主机,后面加上不要启用该环境变量就可以了.
CustomLog "logs/www-access_log" common env=!IMAG
####
```
3. 设置访问日志的权限为root
```
sudo chown -R root.root /application/apache/logs
sudo chmod 700 /application/apache/logs
```
