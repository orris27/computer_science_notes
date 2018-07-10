## 嵌入到Apache的PHP安装

1. 安装好mysql和Apahce
注意:我安装的目录是`/application/mysql`和`/application/apache`
  
2.安装必要的库  
分为yum可以安装的,和只能通过编译安装的libiconv  
```
sudo yum install zlib libxml libjpeg freetype libpng gd curl libiconv zlib-devel libxml2-devel libjpeg-devel freetype-devel libpng-devel gd-devel curl-devel openssl-devel libxslt-devel -y
wget https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.15.tar.gz
tar -zxvf libiconv-1.15.tar.gz 
cd libiconv-1.15/
sudo ./configure --prefix=/usr/local/libiconv-1.15
sudo make && sudo make install
```
3. 安装能以模块的方式嵌入Apache的PHP  
如果libiconv设置为`--prefix=/usr/local/libiconv`的话,就要在下面的配置中写`--with-iconv=/usr/local/libiconv`  
```
sudo ./configure \
--prefix=/application/php-5.6.36 \
--with-apxs2=/application/apache/bin/apxs \
--with-mysql=/application/mysql \
--with-xmlrpc \
--with-openssl \
--with-zlib \
--with-freetype-dir \
--with-gd \
--with-jpeg-dir \
--with-png-dir \
--with-iconv=/usr/local/libiconv \
--enable-short-tags \
--enable-sockets \
--enable-zend-multibyte \
--enable-soap \
--enable-mbstring \
--enable-static \
--enable-gd-native--ttf \
--with-curl \
--with-xsl \
--enable-ftp \
--with-libxml-dir
sudo make && sudo make install
ln -s /application/php-5.6.36/ /application/php
```

4. 检查Apache处是否成功配置  
```
ll /application/apache/modules/ | grep libphp5 # 如果有libphp5.so就说明ok了
grep libphp5 /application/apache/conf/httpd.conf # 如果嵌入一行就说明ok了
```

5. 配置配置文件  
PHP的配置文件  
```
# sudo find / -name 'php.ini*' # 找到当初下载的位置的php.init的两个版本
sudo cp /home/orris/tools/php-5.6.36/php.ini-production  /application/php/lib/php.ini # 拷贝到make后的目录里
```
Apahce的配置文件(使Apache在解析php文件时调用php模块)
```
sudo vim /application/apache/conf/httpd.conf
# 在httpd.conf中添加下面内容
AddType application/x-httpd-php .php .phtml
AddType application/x-httpd-php-source .phps
User www 
Group www
<IfModule dir_module>
    DirectoryIndex index.php index.html
</IfModule>
# :wq 退出到shell中
sudo groupadd www
sudo useradd -g www -M -s /sbin/nologin www
```

6. 重启Apache
```
apachectl -t
sudo apachectl graceful
```

7. 测试是否成功  
添加index.php文件
```
sudo chown -R www.www /var/html # 注意在自己虚拟主机设置的目录下配置,该成自己对应的用户名
sudo touch /var/html/www/index.php
sudo vim /var/html/www/index.php
# 添加下面的内容
<?php
  phpinfo();
?>
# :wq
# 可能还要重启下apache
apachectl  -t
sudo apachectl graceful
```
之后再

### 常见问题
##### 1. configure: error: Cannot find OpenSSL's <evp.h>
解决方法
```
sudo yum install openssl-devel
```
##### 2. configure: error: xslt-config not found. Please reinstall the libxslt >= 1.1.0 distribution
解决方法
```
sudo yum install libxslt-devel
```
##### 3. 配置好Apache的服务后,进入网站是错误的
需要创建index.php文件!!
