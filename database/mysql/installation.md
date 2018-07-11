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
1. 在Linux上添加mysql用户和组
```
sudo useradd mysql -M -s /sbin/nologin
```

1. 下载二进制包  
下载mysql-5.7.22-linux-glibc2.12-x86_64.tar.gz(Linux-Generic就可以了)  


2. 解压
```
sudo mkdir -p /application/mysql-5.7.22
sudo tar -zxvf mysql-5.7.22-linux-glibc2.12-x86_64.tar.gz -C /application/mysql-5.7.22
```

3. 创建软连接,去除版本号
```
sudo ln -s /application/mysql-5.7.22 /application/mysql
```

### 使用
同上面的**使用**
