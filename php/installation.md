## 嵌入到Apache的PHP安装(LAMP)

1. 安装好mysql和Apahce
注意:我安装的目录是`/application/mysql`和`/application/apache`,mysql的版本是5.1.72,apahce是2.4
  
2.安装必要的库  
分为yum可以安装的,和只能通过编译安装的libiconv  
```
sudo yum install zlib libxml2 libjpeg freetype libpng gd curl libiconv zlib-devel libxml2-devel libjpeg-devel freetype-devel libpng-devel gd-devel curl-devel openssl-devel libxslt-devel -y
wget https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.15.tar.gz
tar -zxvf libiconv-1.15.tar.gz 
cd libiconv-1.15/
sudo ./configure --prefix=/usr/local/libiconv-1.15
sudo make && sudo make install
sudo ln -s /usr/local/libiconv-1.15/ /usr/local/libiconv
```
3. 安装能以模块的方式嵌入Apache的PHP  
如果libiconv设置为`--prefix=/usr/local/libiconv`的话,就要在下面的配置中写`--with-iconv=/usr/local/libiconv`  
```
tar -zxvf php-5.6.36.tar.gz 
cd php-5.6.36/
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
```
##### 1. 测试Apache是否成功配置
```
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
##### 2. 测试mysql是否成功配置
```
sudo vim /var/html/www/index.php
# 添加下面的内容
<?php
	//$link_id=mysql_connect(hostname,user,pwd);
	$link_id=mysql_connect('localhost','root','xxx') or mysql_error();
	if($link_id){
		echo "mysql successful by orris !";
	}else{
		echo mysql_error();
	}
?>
```
如果成功的话,就会显示"mysql successful by orris !"

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




## 嵌入到Apache的PHP安装(LNMP)
1. 安装好mysql和Apahce
注意:我安装的目录是`/usr/local/mysql`和`/application/nginx`,mysql的版本是5.7.22,nginx版本是1.14.0
  
2.安装必要的库  
分为yum可以安装的,和只能通过编译安装的libiconv  
```
sudo yum install zlib libxml2 libjpeg freetype libpng gd curl zlib-devel libxml2-devel libjpeg-devel freetype-devel libpng-devel gd-devel curl-devel openssl-devel libxslt-devel -y

wget https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.15.tar.gz
tar -zxvf libiconv-1.15.tar.gz 
cd libiconv-1.15/
sudo ./configure --prefix=/usr/local/libiconv-1.15
sudo make && sudo make install
sudo ln -s /usr/local/libiconv-1.15/ /usr/local/libiconv

# 下面的是跟加密有关的模块(非必需)
sudo yum install libmcrypt libmcrypt-devel -y
sudo yum install mhash mhash-devel -y
sudo yum install mcrypt -y
```

3. 安装fcgi的php  
注意:php5.6的`--with-iconv`应该改成`--with-iconv-dir`!!!
```
tar -zxvf php-5.6.36.tar.gz 
cd php-5.6.36/  523  2018-07-12 11:05:07 ls /usr/local/libiconv
  524  2018-07-12 11:05:38 history | grep libiconv

sudo ./configure \
--prefix=/application/php-5.6.36 \
--with-mysql=/usr/local/mysql \
--with-iconv-dir=/usr/local/libiconv \
--with-freetype-dir \
--with-jpeg-dir \
--with-png-dir \
--with-zlib \
--with-libxml-dir=/usr \
--enable-xml \
--disable-rpath \
--enable-safe-mode \
--enable-bcmath \
--enable-shmop \
--enable-sysvsem \
--enable-inline-optimization \
--with-curl \
--with-curlwrappers \
--enable-mbregex \
--enable-fpm \
--enable-mbstring \
--with-mcrypt \
--with-gd \
--enable-gd-native-ttf \
--with-openssl \
--with-mhash \
--enable-pcntl \
--enable-sockets \
--with-xmlrpc \
--enable-zip \
--enable-soap \
--enable-short-tags \
--enable-zend-multibyte \
--enable-static \
--with-xsl \
--with-fpm-user=nginx \
--with-fpm-group=nginx \
--enable-ftp 
sudo make && sudo make install
ln -s /application/php-5.6.36/ /application/php
```




#### 常见问题
1. checking "whether flock struct is linux ordered"... "no"  
checking "whether flock struct is BSD ordered"... "no"  
configure: error: Don't know how to define struct flock on this system, set --enable-opcache=no  

注意 https://note.t4x.org/error/configure-error-dont-know-how-to-define-struct-flock-on-this-system/
在最后加`--with-libdir=lib64`

2. configure: error: Cannot find libmysqlclient under /usr/local/mysql.
Note that the MySQL client library is not bundled anymore!
一开始出现这个问题是因为我`--with-iconv`的错误,php5.6应该是用`--with-iconv-dir`的格式  
后来又出现这个问题是因为我擅自加了`--with-libdir=lib64`,后来去掉这个就没事了.  
所以最后的configure为:
```
sudo ./configure --prefix=/application/php-5.6.36 --with-mysql=/usr/local/mysql --with-freetype-dir --with-jpeg-dir --with-png-dir --with-zlib --with-libxml-dir=/usr --enable-xml --disable-rpath --enable-safe-mode --enable-bcmath --enable-shmop --enable-sysvsem --enable-inline-optimization --with-curl --with-curlwrappers --enable-mbregex --enable-fpm --enable-mbstring --with-mcrypt --with-gd --enable-gd-native-ttf --with-openssl --with-mhash --enable-pcntl --enable-sockets --with-xmlrpc --enable-zip --enable-soap --enable-short-tags --enable-zend-multibyte --enable-static --with-xsl --with-fpm-user=nginx --with-fpm-group=nginx --enable-ftp --with-iconv-dir=/usr/local/libiconv
```

3. encodings.c:(.text+0xf6): undefined reference to 'libiconv_close' encodings.c:(.text+0x1b6): undefined reference to 'libiconv_close'
找到：
```
EXTRA_LIBS = -lcrypt -lz -lcrypt -lrt -lmysqlclient -lmcrypt -lltdl -lldap -llber -lfreetype -lpng -lz -ljpeg -lcurl -lz -lrt -lm -ldl -lnsl -lrt -lxml2 -lz -lm -lssl -lcrypto -lcurl -lxml2 -lz -lm -lmysqlclient -lm -lrt -ldl -lxml2 -lz -lm -lxml2 -lz -lm -lcrypt -lxml2 -lz -lm -lxml2 -lz -lm -lxml2 -lz -lm -lxml2 -lz -lm -lcrypt
```
在这个行最后面添加 -liconv
```
EXTRA_LIBS = -lcrypt -lz -lcrypt -lrt -lmysqlclient -lmcrypt -lltdl -lldap -llber -lfreetype -lpng -lz -ljpeg -lcurl -lz -lrt -lm -ldl -lnsl -lrt -lxml2 -lz -lm -lssl -lcrypto -lcurl -lxml2 -lz -lm -lmysqlclient -lm -lrt -ldl -lxml2 -lz -lm -lxml2 -lz -lm -lcrypt -lxml2 -lz -lm -lxml2 -lz -lm -lxml2 -lz -lm -lxml2 -lz -lm -lcrypt -liconv
```
重新make一下  
再make install  
4. /home/orris/tools/php-5.6.36/sapi/cli/php: error while loading shared libraries: libiconv.so.2: cannot open shared object file: No such file or directory
make: *** [ext/phar/phar.php] Error 127
解决:  
用命令`sudo find . / -name 'libiconv.so*'`找到libiconv.so的位置,然后添加到`/etc/ld.so.conf`,最后`sudo ldconfig`生效就可以了

5. 
```
Wrote PEAR system config file at: /application/php-5.6.36/etc/pear.conf
You may want to add: /application/php-5.6.36/lib/php to your php.ini include_path
/home/orris/tools/php-5.6.36/build/shtool install -c ext/phar/phar.phar /application/php-5.6.36/bin
cp: cannot stat 'ext/phar/phar.phar': No such file or directory
```
解决办法:  
在下载好了的代码里创建/拷贝该phar.phar文件就可以了
```
cp ext/phar/phar.php ext/phar/phar.phar
```
