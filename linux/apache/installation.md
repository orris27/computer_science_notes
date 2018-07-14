## Apahce-2.4.33

### 安装步骤
1. 安装准备
```
sudo yum install libxml2 libxml2-devel -y

wget http://mirrors.shu.edu.cn/apache//apr/apr-1.6.3.tar.gz
tar -zxvf apr-1.6.3.tar.gz 
cd apr-1.6.3
sudo ./configure --prefix=/usr/local/apr-1.6.3 && \
sudo make && \
sudo make install && \
sudo ln -s /usr/local/apr-1.6.3 /usr/local/apr && \


wget http://apache.mirrors.tds.net//apr/apr-util-1.6.1.tar.gz
tar -zxvf apr-util-1.6.1.tar.gz
cd apr-util-1.6.1
sudo ./configure --prefix=/usr/local/apr-util-1.6.1 --with-apr=/usr/local/apr
sudo make
sudo make install
sudo ln -s /usr/local/apr-util-1.6.1/ /usr/local/apr-util
```

2. 常规步骤
```
wget http://mirrors.hust.edu.cn/apache//httpd/httpd-2.4.33.tar.gz
tar -zxvf httpd-2.4.33.tar.gz
cd httpd-2.4.33
sudo ./configure --prefix=/application/apache-2.4.33 \
--enable-deflate \
--enable-expires \
--enable-headers \
--enable-modules=most \
--enable-so \
--with-mpm=worker \
--enable-rewrite \
--with-apr=/usr/local/apr \
--with-apr-util=/usr/local/apr-util
sudo make && sudo make install
sudo ln -s /application/apache-2.4.33 /application/apache
```
### 使用
```
sudo bin/apachectl -t
sudo bin/apachectl 
```


### 常见问题
1.
```
/usr/local/apr-util-1.6.1/lib/libaprutil-1.so: undefined reference to `XML_GetErrorCode'
/usr/local/apr-util-1.6.1/lib/libaprutil-1.so: undefined reference to `XML_SetEntityDeclHandler'
/usr/local/apr-util-1.6.1/lib/libaprutil-1.so: undefined reference to `XML_ParserCreate'
/usr/local/apr-util-1.6.1/lib/libaprutil-1.so: undefined reference to `XML_SetCharacterDataHandler'
/usr/local/apr-util-1.6.1/lib/libaprutil-1.so: undefined reference to `XML_ParserFree'
/usr/local/apr-util-1.6.1/lib/libaprutil-1.so: undefined reference to `XML_SetUserData'
/usr/local/apr-util-1.6.1/lib/libaprutil-1.so: undefined reference to `XML_StopParser'
/usr/local/apr-util-1.6.1/lib/libaprutil-1.so: undefined reference to `XML_Parse'
/usr/local/apr-util-1.6.1/lib/libaprutil-1.so: undefined reference to `XML_ErrorString'
/usr/local/apr-util-1.6.1/lib/libaprutil-1.so: undefined reference to `XML_SetElementHandler'
collect2: error: ld returned 1 exit status
make[2]: *** [htpasswd] Error 1
make[2]: Leaving directory `/home/orris/tools/httpd-2.4.33/support'
make[1]: *** [all-recursive] Error 1
make[1]: Leaving directory `/home/orris/tools/httpd-2.4.33/support'
make: *** [all-recursive] Error 1
```
解决方法:
```
sudo yum install libxml2-devel -y
rm -rf /usr/local/apr-util
cd /usr/local/src/apr-util-1.6.1
make clean #清除之前配置的缓存
# 后面就是重新configure了
./configure --prefix=/usr/local/apr-util --with-apr=/usr/local/apr
```

2. 
```
xml/apr_xml.c:35:19: fatal error: expat.h: No such file or directory
```
解决方法:
```
sudo yum install expat-devel -y
```
3.
安装了apr-util,执行./configure还是说找不到
```
configure: error: APR-util not found. Please read the documentation.
```
原因:我configure时是`&&`连接了每一个with了,这样是错误的!!  
解决方法:去掉就好了
