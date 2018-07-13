## 0. 优化准备
```
sudo yum install autoconf -y
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
