下载的时候选择Source Code和Linux Generic版本

## mysql-5.1.72
### 步骤
1. 在Linux上添加mysql用户和组
```
sudo groupadd mysql
sudo useradd mysql -g mysql -M -s /sbin/nologin
```

2. 下载源代码,解压并cd
```
wget https://downloads.mysql.com/archives/get/file/mysql-5.1.72.tar.gz
tar -zxvf mysql-5.1.72.tar.gz
cd mysql-5.1.72/
```

3. 设置参数
安装在了`/application/mysql-5.1.72`的位置,可以根据自己需要调整
```
sudo ./configure --prefix=/application/mysql-5.1.72 \
--with-unix-socket-path=/application/mysql-5.1.72/tmp/mysql.sock \
--localstatedir=/application/mysql-5.1.72/data \
--enable-assembler \
--enable-thread-safe-client \
--with-mysqld-user=mysql \
--with-big-tables \
--without-debug \
--with-pthread \
--enable-assembler \
--with-extra-charsets=complex \
--with-readline \
--with-ssl \
--with-embedded-server \
--enable-local-infile \
--with-plugins=partition,innobase \
--with-mysqld-ldflags=-all-static \
--with-client-ldflags=-all-static 
```

4. make
```
sudo make
```

### 常见问题
1. `checking for termcap functions library... configure: error: No curses/termcap library found`

解决方法
```
sudo yum install ncurses-devel
```
