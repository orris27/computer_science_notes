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
--enable-static \一般为将编译成功的可执行文件安装到系统目录中，
--enable-gd-native--ttf \
--with-curl \
--with-xsl \
--enable-ftp \
--with-libxml-dir
sudo make && sudo make install
sudo ln -s /application/php-5.6.36/ /application/php
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
# sudo /application/apach/usr/lib64/mysql/e/bin/apachectl start
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
或者也可以:
```
sudo /application/php/bin/php html/index.php
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

##### 4. 虚拟内存不够用
```
virtual memory exhausted: Cannot allocate memory
make: *** [ext/fileinfo/libmagic/apprentice.lo] Error 1
```
解决方法:(增加一块虚拟内存)  
实际解决方法:1.先`sudo make` 2. 之后等出现这个问题后,直接删除swap文件,然后重新再建一个 3. 不`make clean`,直接`make`(在上一次的基础上make)
注意:好像不是swap越大越好的!!我设置成8G,16G都不行,结果设置成1G,然后不`make clean`,直接`make`就可以了,而且第一个一定是要`/swap`的样子,而不是`/swapfile`
```
sudo dd if=/dev/zero of=/swapfile bs=1M count=1024
sudo mkswap /swapfile
sudo swapon /swapfile
sudo swapon -s # 查看现在的swap情况
```
参考:
```
sudo dd if=/dev/zero of=/swap bs=1024 count=1M 
sudo mkswap /swap 
sudo swapon /swap 
sudo dd if=/dev/zero of=/swapfile bs=1M count=4084
sudo mkswap /swapfile
sudo swapon /swapfile 
sudo dd if=/dev/zero of=/swapfile2 bs=1M count=4084
sudo mkswap /swapfile2
sudo swapon /swapfile2
sudo swapon -s
```

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
cd php-5.6.36/  

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

4. 配置配置文件  
4-1. PHP的配置文件  
```
# sudo find / -name 'php.ini*' # 找到当初下载的位置的php.init的两个版本
sudo cp /home/orris/tools/php-5.6.36/php.ini-production  /application/php/lib/php.ini # 拷贝到make后的目录里
```
4-2. php-fpm的配置文件:  
将php-fpm.conf给配置一下(`.default`为初始的,我们自己配置)  
具体内容可以看我的github上,地址为 https://github.com/orris27/orris/blob/master/php/php-fpm.conf  
然后创建对应目录就可以了:  
```
sudo mkdir -p /app/logs
```

5. 启动php-fpm
```
sudo /application/php/sbin/php-fpm -t # 检查语法
sudo /application/php/sbin/php-fpm
```



6. 测试是否成功  

添加index.php文件
```
sudo touch html/index.php
```
##### 1. 测试Nginx是否成功配置
```
sudo vim html/index.php
# 添加下面的内容
<?php
  phpinfo();
?>
# :wq
sudo vim conf/nginx.conf
#在nginx的server下面改成
location / {
    root   html;
    index  index.php index.html index.htm;
}
location ~ .*\.(php|php5)?$ {
    fastcgi_pass   127.0.0.1:9000;
    fastcgi_index  index.php;
    include fastcgi.conf;
}
# :wq
sudo nginx -t
sudo nginx -s relaod
# 输入对应ip应该就能返回结果了
```
##### 2. 测试mysql是否成功配置
```
sudo vim html/index.php
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


7. 添加到自动启动服务中
```
cat >>/etc/rc.local <<EOF
sudo systemctl start mysqld.service
sudo /application/php/sbin/php-fpm 
sudo /application/nginx/sbin/nginx 
EOF
```

**LNMP配置完成!!!!**


#### 常见问题
1. 
```
checking "whether flock struct is linux ordered"... "no"  
checking "whether flock struct is BSD ordered"... "no"  
configure: error: Don't know how to define struct flock on this system, set --enable-opcache=no  
```
原因:`libmysqlclient.so*`所在的目录不再`/etc/ld.so.conf`里面,所以只要把该目录放到这个文件里面并生效就可以了  
解决方法1:
```
sudo vim /etc/ld.so.conf # 加入'/usr/local/mysql/lib'
sudo ldconfig
```
解决方法2:
改成`--with-iconv-dir=/usr/local/libiconv`,然后把libmysqlclient软连接到/usr/lib下,在`/etc/ld.so.conf`中将`/usr/lib`加入到里面并生效
```
ln -s /usr/local/mysql/lib/libmysqlclient.so* /usr/lib
sudo vim /etc/ld.so.conf # 加入'/usr/lib'
sudo ldconfig
sudo ./configure --prefix=/application/php-5.6.36 --with-mysql=/usr/local/mysql --with-freetype-dir --with-jpeg-dir --with-png-dir --with-zlib --with-libxml-dir=/usr --enable-xml --disable-rpath --enable-safe-mode --enable-bcmath --enable-shmop --enable-sysvsem --enable-inline-optimization --with-curl --with-curlwrappers --enable-mbregex --enable-fpm --enable-mbstring --with-mcrypt --with-gd --enable-gd-native-ttf --with-openssl --with-mhash --enable-pcntl --enable-sockets --with-xmlrpc --enable-zip --enable-soap --enable-short-tags --enable-zend-multibyte --enable-static --with-xsl --with-fpm-user=nginx --with-fpm-group=nginx --enable-ftp --with-iconv-dir=/usr/local/libiconv
```


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
6.
```
Can't connect to local MySQL server through socket '/tmp/mysql.sock' (2)
```

解决办法:  
因为是找不到`mysql.sock`,说明我们在配置mysql的`/etc/my.cnf`时,设置的sock文件在另外一个地方,所以我们先找到该sock文件,然后创建软连接到/tmp下就可以了
```
sudo find / -name 'mysql.sock'
# 输出为/usr/local/mysql-5.7.22-linux-glibc2.12-x86_64/run/mysql.sock
sudo ln -s /usr/local/mysql/run/mysql.sock /tmp/
```
7. yum不能allocate memory
解决方法:暂停mysql服务
```
sudo systemctl stop mysqld.service
```
