注意:如果是mysql和php分开安装,则在php的服务器上要么编译安装到make install,要么压缩包解压就ok了
## 安装思路
1. 编译安装make(下载的时候选择Source Code和Linux Generic版本)  
2. yum/rpm : 参考https://www.cnblogs.com/brianzhu/p/8575243.html
3. 下载二进制包(包名为mysql-5.5.32-linux2.6-x86_64.tar.gz)  
4. 编译安装cmake
## mysql-5.1.72(编译安装make)
### 安装步骤
0. 安装准备
```
sudo yum install ncurses-devel -y
sudo yum groupinstall "Development Tools" -y
```

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

5. make install
```
sudo make install
```

6. 创建软连接,去掉版本号
```
sudo ln -s /application/mysql-5.1.72/ /application/mysql
```



### 使用
1. 替换mysql的配置文件为对应硬件支持的配置文件
```
sudo cp /etc/my.cnf /etc/my.cnf.bak # make a copy for a previous mysql
sudo cp /application/mysql/share/mysql/my-small.cnf /etc/my.cnf
```

2. 创建存放数据的文件
```
sudo mkdir data # data为/application/mysql/data
sudo chown -R mysql.mysql data/
```

3. 初始化存放数据的文件  
```
sudo /application/mysql/bin/mysql_install_db --basedir=/application/mysql --datadir=/application/mysql/data/ --user=mysql
```


4. 启动mysql服务器
```
sudo /application/mysql/bin/mysqld_safe &
```

6. 进入mysql客户端
```
sudo /application/mysql/bin/mysql
```

5. 将mysql命令添加到系统环境变量中
```
sudo vim /etc/profile
# 添加 'PATH=/application/mysql/bin:$PATH' 到里面就可以了
. /etc/profile
```

6. 添加到sudo命令中
```
sudo visudo
# 添加 mysql/bin目录到$PATH中就可以了
# 如改成 Defaults    secure_path = /application/mysql/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin 就可以了
```

7. 进入mysql客户端并设置密码和删除不必要的用户
```

/application/mysql/bin/mysqladmin -u root password 'new-password'
sudo mysql -uroot -p

select user,host from mysql.user;
drop user ""@centos1;
drop user ""@localhost;
drop user "root"@centos1;
```

### 暂停mysql服务
```
sudo mysqladmin shutdown
```
### 常见问题
##### 1. `checking for termcap functions library... configure: error: No curses/termcap library found`

解决方法
```
sudo yum install ncurses-devel -y
```

##### 2. 添加到/etc/init.d的mysqld服务不能启动
原因:mysql_safe和mysql.server可能不能一起使用
解决方法
1. 重新初始化mysql存放数据的目录
```
sudo rm -rf /application/mysql/data/
sudo /application/mysql/bin/mysql_install_db --basedir=/application/mysql --datadir=/application/mysql/data/ --user=mysql
sudo chown -R mysql.mysql data
sudo chmod 755 data
```
2. 添加自定义系统服务到系统服务中  
拷贝mysql.server到/etc/init.d下,为mysqld  
设置mysqld的权限为可执行,如755  
chkconfig add该mysqld  
设置开机自动启动 chkconfig on  
之后就可以/etc/init.d/mysqld start启动了  

##### 3. 
```
../depcomp: line 571: exec: g++: not found
make[1]: *** [my_new.o] Error 127
make[1]: Leaving directory `/home/orris/tools/mysql-5.1.72/mysys'
make: *** [all-recursive] Error 1
```
解决方法:(g++ not found=>没有安装g++=>安装开发者工具)
```
sudo yum groupinstall "Development Tools" -y
```

##### 5.
```
../include/my_global.h:1110:15: error: redeclaration of C++ built-in type ‘bool’ [-fpermissive]
```
解决方法:(redeclaration=>重新编译就好了)
```
sudo make clean
sudo ./configure ...
sduo make && sudo make install 
...
```


## mysql-5.7.22(二进制包安装)

### 安装步骤
具体可以参考 https://www.jianshu.com/p/0d628b2f7476  

0. 安装准备
```
sudo yum install libaio-devel -y
```

1. 在Linux上添加mysql用户和组  
```
sudo useradd mysql -M -s /sbin/nologin
```

1. 下载二进制包  
下载mysql-5.7.22-linux-glibc2.12-x86_64.tar.gz(Linux-Generic就可以了)  


2. 解压
```
tar -zxvf mysql-5.7.22-linux-glibc2.12-x86_64.tar.gz
sudo mv mysql-5.7.22-linux-glibc2.12-x86_64 /usr/local/
```

3. 创建软连接,去除版本号
```
sudo ln -s /usr/local/mysql-5.7.22-linux-glibc2.12-x86_64/ /usr/local/mysql
```

### 使用
1. 创建需要的目录
```
sudo mkdir -p /usr/local/mysql/{data,binlogs,logs,etc,run}
sudo chown -R mysql.mysql /usr/local/mysql/{data,binlogs,logs,etc,run}
```

2. 替换mysql的配置文件为对应硬件支持的配置文件
```
sudo cp /etc/my.cnf /etc/my.cnf.bak # make a copy for a previous mysql
sudo vim /etc/my.cnf
# 在/etc/my.cnf中输入下面的内容
```
/etc/my.cnf:
```
[client]
port = 3306
socket = /usr/local/mysql/run/mysql.sock

[mysqld]
port = 3306
socket = /usr/local/mysql/run/mysql.sock
pid_file = /usr/local/mysql/run/mysqld.pid
datadir = /usr/local/mysql/data
default_storage_engine = InnoDB
max_allowed_packet = 512M
max_connections = 2048
open_files_limit = 65535

skip-name-resolve
lower_case_table_names=1

character-set-server = utf8mb4
collation-server = utf8mb4_unicode_ci
init_connect='SET NAMES utf8mb4'


innodb_buffer_pool_size = 1024M
innodb_log_file_size = 2048M
innodb_file_per_table = 1
innodb_flush_log_at_trx_commit = 0


key_buffer_size = 64M

log-error = /usr/local/mysql/logs/mysql_error.log
log-bin = /usr/local/mysql/binlogs/mysql-bin
slow_query_log = 1
slow_query_log_file = /usr/local/mysql/logs/mysql_slow_query.log
long_query_time = 5


tmp_table_size = 32M
max_heap_table_size = 32M
query_cache_type = 0
query_cache_size = 0

server-id=1
```

4. 初始化存放数据的文件(`--initialize`是会产生随机密码的,而如果使用`--initialize-insecure`的话,就是初始无密码)
```
sudo /usr/local/mysql/bin/mysqld --initialize --basedir=/usr/local/mysql --datadir=/usr/local/mysql/data/ --user=mysql
```


注意root需要密码是因为目录初始化有问题,所以老师的建议是删除数据data目录,然后重新初始化

5. 将mysql命令添加到系统环境变量中
```
sudo vim /etc/profile
# 添加 'PATH=/usr/local/mysql/bin:$PATH' 到里面就可以了
source /etc/profile
```

6. 添加到sudo命令中
```
sudo visudo
# 添加 mysql/bin目录到$PATH中就可以了
# 如改成 Defaults    secure_path = /usr/local/mysql/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin 就可以了
```

7. 进入mysql客户端
```
sudo mysql -uroot -p
```

8. 添加mysql到systemctl中
```
cd /usr/lib/systemd/system
sudo touch mysqld.service
sudo vim mysqld.service # 添加的内容看后面
sudo systemctl daemon-reload 
sudo systemctl enable mysqld.service 
sudo systemctl is-enabled mysqld.service
sudo systemctl start mysqld.service 
```
mysqld.service:
```
# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#
# systemd service file for MySQL forking server
#

[Unit]
Description=MySQL Server
Documentation=man:mysqld(8)
Documentation=http://dev.mysql.com/doc/refman/en/using-systemd.html
After=network.target
After=syslog.target

[Install]
WantedBy=multi-user.target

[Service]
User=mysql
Group=mysql

Type=forking

PIDFile=/usr/local/mysql/run/mysqld.pid

# Disable service start and stop timeout logic of systemd for mysqld service.
TimeoutSec=0

# Execute pre and post scripts as root
PermissionsStartOnly=true

# Needed to create system tables
#ExecStartPre=/usr/bin/mysqld_pre_systemd

# Start main service
ExecStart=/usr/local/mysql/bin/mysqld --daemonize --pid-file=/usr/local/mysql/run/mysqld.pid $MYSQLD_OPTS

# Use this to switch malloc implementation
EnvironmentFile=-/etc/sysconfig/mysql

# Sets open_files_limit
LimitNOFILE = 65535

Restart=on-failure

RestartPreventExitStatus=1

PrivateTmp=false
```


9. 进入mysql客户端
```
# 在/usr/local/mysql/logs/mysql_error.log这个目录里会有
# 2018-07-11T15:35:19.962367Z 1 [Note] A temporary password is generated for root@localhost: /(/+AhNR#3oJ
# 这样的内容,这就是root的临时密码,所以要用该临时密码登入
# 在mysql下修改密码
sudo cat /usr/local/mysql/logs/mysql_error.log | grep temporary
sudo /usr/local/mysql/bin/mysql -uroot -p\/\(\/+AhNR#3oJ
set password=password('new_password');
```




### 常见问题
#### 1.
```
/usr/local/mysql/bin/mysqld: error while loading shared libraries: libaio.so.1: cannot open shared object file: No such file or directory
```
解决方法:(要么是没有安装,要么是没有找到)
```
sudo yum install libaio-devel -y
```
#### 2. 权限不够或找不到文件
##### 解决方法
1. 创建出所有需要的目录
2. 授权所有目录
3. 如果还是权限不够,说明还要创建并授权指定的文件,如`mysql-error.log`.不仅是目录,连文件也要自己创建出来


## mysql-5.7.22(编译安装cmake)
### 思路
一般编译安装的包都是"产品名+版本号.tar.gz",其他有多的字符很可能是其他类型的发行版.比如说cmake就有`cmake-3.12.0-rc3.tar.gz`版本.和数据库一样,这个不是最终发布的版本
### 安装步骤
1. 安装cmake软件
```
wget https://cmake.org/files/v3.11/cmake-3.11.4.tar.gz
tar -zxf cmake-3.11.4.tar.gz 
cd cmake-3.11.4
sudo ./configure 
gmake && gmake install
```
2. 安装依赖包(我中途还安装了bison,不知道需不需要)
```
sudo yum install ncurses-devel -y
# boost
wget https://sourceforge.net/projects/boost/files/boost/1.59.0/boost_1_59_0.tar.gz
tar zxf boost_1_59_0.tar.gz 
sudo mv boost_1_59_0 /usr/local/
sudo ln -s /usr/local/boost_1_59_0/ /usr/local/boost
```
3. 创建用户和组
```
sudo groupadd mysql
sudo useradd -g mysql -M -s /sbin/nologin mysql
```
3. 编译安装
```
wget https://dev.mysql.com/get/Downloads/MySQL-5.7/mysql-5.7.22.tar.gz
tar -zxf mysql-5.7.22.tar.gz
cd mysql-5.7.22

sudo cmake . -DCMAKE_INSTALL_PREFIX=/application/mysql-5.7.22 \
-DMYSQL_DATADIR=/application/mysql-5.7.22/data \
-DMYSQL_UNIX_ADDR=/application/mysql-5.7.22/tmp/mysql.sock \
-DDEFAULT_CHARSET=utf8mb4 \
-DDEFAULT_COLLATION=utf8mb4_general_ci \
-DEXTRA_CHARSETS=gbk,gb2312,utf8,ascii \
-DENABLED_LOCAL_INFILE=ON \
-DWITH_INNOBASE_STORAGE_ENGINE=1 \
-DWITH_FEDERATED_STORAGE_ENGINE=1 \
-DWITH_BLACKHOLE_STORAGE_ENGINE=1 \
-DWITHOUT_EXAMPLE_STORAGE_ENGINE=1 \
-DWITHOUT_PARTITION_STORAGE_ENGINE=1 \
-DWITH_FAST_MUTEXES=1 \
-DWITH_ZLIB=bundled \
-DENABLED_LOCAL_INFILE=1 \
-DWITH_READLINE=1 \
-DWITH_EMBEDDED_SERVER=1 \
-DWITH_DEBUG=0 \
-DWITH_BOOST=/usr/local/boost
sudo make && sudo make install
sudo ln -s /application/mysql-5.7.22/ /application/mysql
```
4. 配置数据库的配置文件
+ 官网说：从5.7.18开始不在二进制包中提供my-default.cnf文件，不再需要my.cnf。从源码编译安装的mysql,基本配置在预编译和systemd启动文件中已定义好，不用my.cnf配置。
```
sudo cp /etc/my.cnf /etc/my.cnf.bak # make a copy for a previous mysql
sudo vim /etc/my.cnf
# 在/etc/my.cnf中输入下面的内容
```
/etc/my.cnf:
> https://github.com/orris27/orris/blob/master/database/mysql/my_cnf.md


5. 配置环境变量,并添加到sudo中
+ 注意:如果mysql/bin放到$PATH后面的话,可能导致mysqldump等命令出问题
```
sudo vim /etc/profile
####
PATH=/application/mysql/bin:$PATH
####
source /etc/profile
sudo visudo
# 添加到visudo里面
```

6. 初始化数据文件
```
sudo mkdir /application/mysql/{data,logs,run,tmp}
sudo chown -R mysql.mysql /application/mysql/{data,logs,run,tmp}
sudo chmod -R 1777 /tmp/
sudo mysqld --initialize-insecure --user=mysql --basedir=/application/mysql --datadir=/application/mysql/data/
```

7. 添加mysql到systemctl中
```
cd /usr/lib/systemd/system
sudo touch mysqld.service
sudo vim mysqld.service # 添加的内容看后面
sudo systemctl daemon-reload 
sudo systemctl enable mysqld.service 
sudo systemctl is-enabled mysqld.service
sudo systemctl start mysqld.service # 如果启动不了的话,检查/etc/my.cnf中所有需要的目录是否都创建并授权
```
mysqld.service
> https://github.com/orris27/orris/blob/master/database/mysql/mysqld_service.md

8. 进入mysql客户端
```
sudo mysql
set password=password('new_password');
```



### 常见问题
#### 1. 需要安装的boost是1.59.0,而不是最新版
```
-- BOOST_INCLUDE_DIR /usr/local/boost
-- LOCAL_BOOST_DIR /usr/local/boost
-- LOCAL_BOOST_ZIP LOCAL_BOOST_ZIP-NOTFOUND
-- Could not find (the correct version of) boost.
-- MySQL currently requires boost_1_59_0

CMake Error at cmake/boost.cmake:81 (MESSAGE):
  You can download it with -DDOWNLOAD_BOOST=1 -DWITH_BOOST=<directory>

  This CMake script will look for boost in <directory>.  If it is not there,
  it will download and unpack it (in that directory) for you.

  If you are inside a firewall, you mad /usr/local/mysy need to use an http proxy:

  export http_proxy=http://example.com:80

Call Stack (most recent call first):
  cmake/boost.cmake:269 (COULD_NOT_FIND_BOOST)
  CMakeLists.txt:506 (INCLUDE)


-- Configuring incomplete, errors occurred!
See also "/home/orris/tools/mysql-5.7.22/CMakeFiles/CMakeOutput.log".
See also "/home/orris/tools/mysql-5.7.22/CMakeFiles/CMakeError.log".
```
解决方法:
```
wget https://sourceforge.net/projects/boost/files/boost/1.59.0/boost_1_59_0.tar.gz
tar zxf boost_1_59_0.tar.gz 
sudo mv boost_1_59_0 /usr/local/
sudo ln -s /usr/local/boost_1_59_0/ /usr/local/boost
```
#### 2. 权限不够或找不到文件
##### 解决方法
1. 创建出所有需要的目录
2. 授权所有目录
3. 如果还是权限不够,说明还要创建并授权指定的文件,如`mysql-error.log`.不仅是目录,连文件也要自己创建出来
