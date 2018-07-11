下载的时候选择Source Code和Linux Generic版本  
## 安装思路
1. 编译安装  
2. yum/rpm  
3. 下载二进制包(包名为mysql-5.5.32-linux2.6-x86_64.tar.gz)  
## mysql-5.1.72(编译安装)
### 安装步骤
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

### 常见问题
##### 1. `checking for termcap functions library... configure: error: No curses/termcap library found`

解决方法
```
sudo yum install ncurses-devel
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

### 使用
1. 替换mysql的配置文件为对应硬件支持的配置文件
```
sudo cp /etc/my.cnf /etc/my.cnf.bak # make a copy for a previous mysql
sudo cp share/mysql/my-small.cnf /etc/my.cnf
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
# 添加 PATH="/application/mysql/bin:$PATH" 到里面就可以了
. /etc/profile
```

6. 添加到sudo命令中
```
su - root
visudo
# 添加 mysql/bin目录到$PATH中就可以了
# 如改成 Defaults    secure_path = /application/mysql/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin 就可以了
```

7. 进入mysql客户端
```
sudo mysql
```

### 暂停mysql服务
```
sudo mysqladmin shutdown
```



## mysql-5.7.22(二进制包安装)

### 安装步骤
具体可以参考 https://www.jianshu.com/p/0d628b2f7476
1. 在Linux上添加mysql用户和组  
```
sudo useradd mysql -M -s /sbin/nologin
```

1. 下载二进制包  
下载mysql-5.7.22-linux-glibc2.12-x86_64.tar.gz(Linux-Generic就可以了)  


2. 解压
```
tar -zxvf mysql-5.7.22-linux-glibc2.12-x86_64.tar.gz
sudo mv mysql-5.7.22-linux-glibc2.12-x86_64 /application/
```

3. 创建软连接,去除版本号
```
sudo ln -s /application/mysql-5.7.22-linux-glibc2.12-x86_64/ /application/mysql
```

### 使用
1. 创建需要的目录
```
mkdir -p /usr/local/mysql/{data,binlogs,logs,etc,run}
chown -R mysql.mysql /usr/local/mysql/{data,binlogs,log,etc,run}
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
pid_file = /usr/local/mysql/run/mysql.pid
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

4. 初始化存放数据的文件  
```
sudo /usr/local/mysql/bin/mysqld --initialize --basedir=/usr/local/mysql --datadir=/usr/local/mysql/data/ --user=mysql
```

5. 启动mysql服务器
```
sudo /application/mysql/bin/mysqld_safe &
```

6. 进入mysql客户端
```
# 在/usr/local/mysql/log/mysql_error.log这个目录里会有
# 2018-07-11T15:35:19.962367Z 1 [Note] A temporary password is generated for root@localhost: /(/+AhNR#3oJ
# 这样的内容,这就是root的临时密码,所以要用该临时密码登入
sudo /application/mysql/bin/mysql -uroot -p\/\(\/+AhNR#3oJ
# 在mysql下修改密码
set password=password('new_password');
```
注意root需要密码是因为目录初始化有问题,所以老师的建议是删除数据data目录,然后重新初始化

7. 将mysql命令添加到系统环境变量中
```
sudo vim /etc/profile
# 添加 PATH="/usr/local/mysql/bin:$PATH" 到里面就可以了
. /etc/profile
```

8. 添加到sudo命令中
```
su - root
visudo
# 添加 mysql/bin目录到$PATH中就可以了
# 如改成 Defaults    secure_path = /application/mysql/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin 就可以了
```

9. 进入mysql客户端
```
sudo mysql -uroot -p
```

10. 添加mysql到systemctl中
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


