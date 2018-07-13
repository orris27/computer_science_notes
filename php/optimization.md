## 0. 优化准备
```
sudo yum install autoconf -y
echo 'export LC_ALL=C' >> /etc/profile
source /etc/profile
sudo yum install perl-devel -y
```

## 1. PHP引擎加速与缓存优化模块
### <1> eaccelerator
好像不能用...
### <2> xcache
```
wget https://xcache.lighttpd.net/pub/Releases/3.2.0/xcache-3.2.0.tar.gz
tar -zxvf xcache-3.2.0.tar.gz
cd xcache-3.2.0
sudo /application/php/bin/phpize 
sudo ./configure --enable-xcache --with-php-config=/application/php/bin/php-config
sudo make && sudo make install
ls /application/php/lib/php/extensions/no-debug-non-zts-20131226/ # 注意该目录不一定,根据make install的提示来看,如果有xcache.so就ok了
```
## 2. PHP(甚至其他语言)访问数据库做缓存
```
wget https://pecl.php.net/get/memcache-3.0.8.tgz
tar -zxf memcache-3.0.8.tgz 
cd memcache-3.0.8
sudo /application/php/bin/phpize 
sudo ./configure --with-php-config=/application/php/bin/php-config
sudo make && sudo make install
ll /application/php/lib/php/extensions/no-debug-zts-20131226/
```

## 3. PHP访问数据库的新接口(不是优化,属于扩展功能)
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

## 4. 图像处理
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

## 配置到PHP中
LNMP要重启php-fpm,而LAMP要重启apache
### 1. LAMP
#### 1. PHP自身模块(memcache,pdo_mysql,imagick)
##### 思路
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
#### 2. xcache
##### 思路
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

```

### 2. LNMP
#### 1. PHP自身模块(memcache,pdo_mysql,imagick)
##### 思路
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
#### 2. xcache
##### 思路
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
```

## 常见问题
### 1.没有`./configure`文件
原因: `./configure`文件只有在`phpize`后才出现

### 2. 
phpize的时候出现下面信息
```
Cannot find autoconf. Please check your autoconf installation and the
$PHP_AUTOCONF environment variable. Then, rerun this script.
```
解决方法:
```
sudo yum install autoconf -y
```

### 3. 
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

