### 1. 加载模块
#### 1-1. 优化准备
```
sudo yum install autoconf -y
echo 'export LC_ALL=C' >> /etc/profile
source /etc/profile
sudo yum install perl-devel -y
```

#### 1-2. PHP引擎加速与缓存优化模块
##### <1> eaccelerator
好像不能用...
##### <2> xcache
```
wget https://xcache.lighttpd.net/pub/Releases/3.2.0/xcache-3.2.0.tar.gz
tar -zxvf xcache-3.2.0.tar.gz
cd xcache-3.2.0
sudo /application/php/bin/phpize 
sudo ./configure --enable-xcache --with-php-config=/application/php/bin/php-config
sudo make && sudo make install
ls /application/php/lib/php/extensions/no-debug-non-zts-20131226/ # 注意该目录不一定,根据make install的提示来看,如果有xcache.so就ok了
```
#### 1-3. PHP(甚至其他语言)访问数据库做缓存
```
wget https://pecl.php.net/get/memcache-3.0.8.tgz
tar -zxf memcache-3.0.8.tgz 
cd memcache-3.0.8
sudo /application/php/bin/phpize 
sudo ./configure --with-php-config=/application/php/bin/php-config
sudo make && sudo make install
ll /application/php/lib/php/extensions/no-debug-zts-20131226/
```
##### 4. phpredis (需要redis的话安装)
```
wget https://github.com/nicolasff/phpredis/archive/master.zip
unzip master.zip
cd phpredis-master/

sudo /application/php/bin/phpize 
sudo ./configure --with-php-config=/application/php/bin/php-config 
sudo make && sudo make install
ll /application/php-5.6.36/lib/php/extensions/no-debug-non-zts-20131226/
sudo vim /application/php/lib/php.ini
###
extension = redis.so
###
sudo pkill php-fpm
sudo /application/php/sbin/php-fpm 
```
##### 4-1. 检查
通过phpinfo来看是否有redis模块.注意:`php.ini`里面的`disable_functions`如果设置了`phpinfo`就不能使用了!!!!
#### 1-4. PHP访问数据库的新接口(不是优化,属于扩展功能)
```
wget https://pecl.php.net/get/PDO_MYSQL-1.0.2.tgz

# 注意:根据MySQL版本不同,这里的具体位置会不一样!!
sudo ln -s /application/mysql/include/mysql/* /usr/local/include/ # 将mysql的头文件放到gcc查找的默认路径

tar -zxf PDO_MYSQL-1.0.2.tgz
cd PDO_MYSQL-1.0.2
sudo /application/php/bin/phpize 
sudo ./configure --with-php-config=/application/php/bin/php-config --with-pdo-mysql=/application/mysql # mysql的目录不要弄错
sudo make 
sudo make install
ls /application/php/lib/php/extensions/no-debug-zts-20131226/

```

#### 1-5. 图像处理
```
wget https://www.imagemagick.org/download/ImageMagick.tar.gz
tar -zxf ImageMagick.tar.gz
cd ImageMagick-7.0.8-6
sudo ./configure
sudo make && sudo make install

wget https://pecl.php.net/get/imagick-3.4.3.tgz
tar -zxf imagick-3.4.3.tgz 
cd imagick-3.4.3
sudo /application/php/bin/phpize 
sudo ./configure --with-php-config=/application/php/bin/php-config
sudo make && sudo make install
ll /application/php/lib/php/extensions/no-debug-zts-20131226/
```

#### 1-6. 配置到PHP中
LNMP要重启php-fpm,而LAMP要重启apache
##### 1-6-1. LAMP
###### 1-6-1-1. PHP自身模块(memcache,pdo_mysql,imagick)
思路  
1. 修改`php.ini`文件(确定路径+名字),将`extenson_dir`改成`/application/php/lib/php/extensions/no-debug-zts-20131226/`这样的路径,然后在后面加上具体文件,如`extension = memcache.so`(注意要等号两边有空格),
2. 重启apache服务
3. 访问下phpinfo(),测试就行了(生产环境中一定要删除phpinfo()的东西)
```
sudo cp /application/php/lib/php.ini /application/php/lib/php.ini.bak
sudo vim /application/php/lib/php.ini                                

extension_dir = "/application/php/lib/php/extensions/no-debug-zts-20131226/"
#--vim--
;--cache ext start --
extension = memcache.so
extension = pdo_mysql.so
extension = imagick.so
;--cache ext end --
#--vim--

sudo /application/apache/bin/apachectl graceful
```
###### 1-6-1-2. xcache
思路
1. 修改下载解压的目录下的xcache的配置文件,将里面的参数改成适合的参数
2. 将xcache的配置文件直接追加到php的配置文件里
3. 创建并授权用来做缓存的目录(授权是给对应我们Web服务指定的用户,比如nginx为nginx,apache为www(配置文件里设置的))
4. 重启相关服务
```
sudo vim ~/tools/xcache-3.2.0/xcache.ini
#--vim starts--
extension = xcache.so
xcache.size  =               128M
xcache.count =                 2
xcache.ttl   =                 86400
xcache.gc_interval =          3600
xcache.var_size  =            0
#--vim ends--
复制整个xcache.ini到php.ini里面
sudo /application/apache/bin/apachectl graceful
/application/php/bin/php -v
```

##### 1-6-2. LNMP
###### 1-6-2-1. PHP自身模块(memcache,pdo_mysql,imagick)
思路
1. 修改`php.ini`配置文件,加入这些新的扩展功能
2. 重启php-fpm
3. 随便弄个网站搞出phpinfo(),然后用搜索功能差看memcache,pdo_mysql等,有就说明成功了
```
sudo cp /application/php/lib/php.ini  /application/php/lib/php.ini.bak
sudo vim /application/php/lib/php.ini
#--vim--
extension_dir = "/application/php/lib/php/extensions/no-debug-non-zts-20131226/"
;--cache ext start --
extension = memcache.so
extension = pdo_mysql.so
extension = imagick.so
;--cache ext end --
#--vim--

sudo netstat -lntup | grep php-fpm
sudo kill -USR2 `cat /app/logs/php-fpm.pid`
sudo /application/php/sbin/php-fpm

sudo vim /application/apache/htdocs/index.php
#--vim--
<?php
 phpinfo();
?>
#--vim--
```
#### 1-6-2-2. xcache
思路
1. 修改下载解压的目录下的xcache的配置文件,将里面的参数改成适合的参数
2. 将xcache的配置文件直接追加到php的配置文件里
3. 创建并授权用来做缓存的目录(授权是给对应我们Web服务指定的用户,比如nginx为nginx,apache为www(配置文件里设置的))
4. 重启相关服务
```
sudo vim ~/tools/xcache-3.2.0/xcache.ini
#--vim starts--
extension = xcache.so
xcache.size  =               128M
xcache.count =                 2
xcache.ttl   =                 86400
xcache.gc_interval =          3600
xcache.var_size  =            0
#--vim ends--
复制整个xcache.ini到php.ini里面
sudo kill -USR2 `cat /app/logs/php-fpm.pid`
sudo /application/php/sbin/php-fpm
/application/php/bin/php -v
```

#### 常见问题
##### 1.没有`./configure`文件
原因: `./configure`文件只有在`phpize`后才出现

##### 2. 
phpize的时候出现下面信息
```
Cannot find autoconf. Please check your autoconf installation and the
$PHP_AUTOCONF environment variable. Then, rerun this script.
```
解决方法:
```
sudo yum install autoconf -y
```

##### 3. 
```
/home/orris/tools/PDO_MYSQL-1.0.2/php_pdo_mysql_int.h:25:19: fatal error: mysql.h: No such file or directory
 #include <mysql.h>
                   ^
compilation terminated.
make: *** [pdo_mysql.lo] Error 1
```
原因: 找不到mysql.h文件. 说明gcc编译时的系统默认路径中没有该文件  
解决方法:(将mysql.h所在的目录下所有mysql的头文件都加入到系统默认路径中`/usr/include;/usr/local/include`)
```
sudo ln -s /application/mysql/include/mysql/* /usr/local/include/
```

### 2. 打开php的安全模式(PHP5.4以上已经不支持了,需要自己调整)
php的安全模式能够控制某些函数的执行,如system;并且对很多文件操作的函数进行了权限控制
#### 解决方法
```
safe_mode=On
safe_mode_gid=Off
```
### 3. 关闭危险函数
我们不希望执行一些危险函数,如`phpinfo`,`system`等
```
disable_functions = system, passthru, exec, shell_exec, popen, phpinfo
```
#### 3-1. 注意
如果我们访问`index.php`而认为没有响应时,看看phpinfo是不是被禁止掉了!!(如果是lnmp的话,最后要重启php-fpm)
### 4. 隐藏php版本
#### 解决方法
```
expose_php = Off
```
### 5. 防止SQL注入(php5.6没有)
打开magic_quotes_gpc,他会自动将输入的字符串里的\`转换为\\\`的形式
#### 解决方法
```
magic_quotes_gpc=On
```
### 6. 错误信息控制
不显示提示错误(生产环境)
```
display_errors = Off
```
### 7. 错误日志
如果关闭了错误提示,就将错误记录在日志里.  
注意:给文件必须允许Apache的用户和组具有写的权限
```
log_errors=On
error_log=/app/logs/php-errors.log
```
### 8. 部分资源限制
1. 限制一个脚本执行的最长时间(灵活)
```
max_execution_time=30
```
2. 限制一个脚本使用的最大内存
```
memory_limit=128M
```
3. 限制一个脚本等待输入的最长时间
```
max_input_time=60
```
4. 限制上传文件的大小
```
upload_max_filesize=2M
```
### 9. 禁止打开远程地址
```
allow_url_fopen=Off
cgi.fix_pathinfo=0 # 防止Nginx文件类型错误解析漏洞
```
### 10. 调整session存放位置(会话保持)
1. 安装`memcache`模块
2. 配置文件中添加模块的目录
3. 配置session参数
#### 检验
1. 是否有`so`文件
```
ll /application/php/lib/php/extensions/no-debug-zts-20131226/
# ll /application/php/lib/php/extensions/no-debug-non-zts-20131226/ # 根据PHP不同编译情况,会有差别
```
2. 是否配置了memcache
+ `extension_dir`目录下是否有so文件
- `extension_dir = "/application/php/lib/php/extensions/no-debug-non-zts-20131226/"`
+ 如果配置session的话,`php.ini`中的`session.save_handler`和`session.save_path`是否调整好
- `session.save_handler = memcache`
- `session.save_path = "tcp://172.19.28.82:11211"`
