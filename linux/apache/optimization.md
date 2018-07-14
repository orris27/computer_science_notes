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
gai
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
1. 将模块添加到Apache的配置文件中
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
2. 平滑重启Apachegai
```
sudo apachectl -t
sudo apachectl graceful
```

### 6. 缓存的过期实际那
#### 缓存依据
1. 不希望被缓存的
+ 广告图片
+ 更新频繁的文件(google的logo)
+ 网站流量统计的工具
2. 缓存标准(淘宝为例)
+ 普通的js(10年)
+ 频繁更新的js(1天)
+ 流量统计的js(没有缓存)
+ 普通图片(10年)
#### 查看某个网站的缓存
YSlow的components里可以看到
#### 配置
可以通过配置不同虚拟主机,不同的目录来设置同一类型文件的不同过期时间
```
# server标签下
location ~ .*\.(gif|jpg|jpeg|png|bmp|swf)?$ {
    root html;
    expires 3650d;
}
location ~ .*\.(js|css)?$ {
    root html;
    expires 30d;
}
location ~ (robots.txt) {
    log_not_found off;
    expires 7d;
    break;
}
```

