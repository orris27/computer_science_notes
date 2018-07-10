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
```


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
