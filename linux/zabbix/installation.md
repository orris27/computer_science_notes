## 1. 安装和配置(rpm)
> https://www.zabbix.com/download
### 1-1. 配置zabbix的yum源
将zabbix的rpm包放到CentOS的yum源下.
+ yum源的内容通过`ls /etc/yum.repos.d/`和`rpm -ql zabbix-release`查看
+ rpm的参数为`-ivh`
```
sudo rpm -ivh https://repo.zabbix.com/zabbix/3.4/rhel/7/x86_64/zabbix-release-3.4-2.el7.noarch.rpm
ls /etc/yum.repos.d/
rpm -ql zabbix-release
```
### 1-2. 通过配置好的yum源直接yum安装
```
sudo yum install -y zabbix-server zabbix-web zabbix-server-mysql zabbix-web-mysql zabbix-agent
```


### 1-3. zabbix数据库
#### 1-3-1. 创建zabbix使用的数据库
```
mysql -uroot -p
###
create database zabbix character set utf8 collate utf8_bin;
###
```
#### 1-3-2. 授权zabbix用户
```
###
grant all privileges on zabbix.* to zabbix@localhost identified by 'zabbix';
###
```
#### 1-3-3. 导入zabbix使用的schema,data
```
zcat /usr/share/doc/zabbix-server-mysql*/create.sql.gz | mysql -uzabbix -p zabbix
```
#### 1-3-4. 配置zabbix的配置文件,使zabbix连接某个数据库
前面的三个操作只是单方面在数据库做好准备,现在要让zabbix去连接MySQL
+ 一般只要修改下面四个参数就ok了
+ 其他需求如指定MySQL的socket的话,也可以在`/etc/zabbix/zabbix_server.conf`中调整参数
```
sudo vim /etc/zabbix/zabbix_server.conf
###
DBHost=localhost
DBName=zabbix
DBUser=zabbix
DBPassword=zabbix
###
```
### 1-4. 配置zabbix的时区
```
sudo vim /etc/httpd/conf.d/zabbix.conf
###
php_value date.timezone Asia/Shanghai
###
```

### 1-5. Start Zabbix server and agent processes
Start Zabbix server and agent processes and make it start at system boot
```
sudo systemctl restart zabbix-server zabbix-agent httpd
# sudo systemctl enable zabbix-server zabbix-agent httpd
```

### 1-6. 安装Web的Zabbix
1. 访问`http://47.100.185.187/zabbix/setup.php`.(ip地址为安装Zabbix的服务器的IP地址)
2. 在`Check of pre-requisites`里,所有都要OK,并且`PHP databases support`为MySQL
3. 配置数据库连接.端口0表示使用默认的端口,即3306
+ 如果是外网连接的话,授权的zabbix的host要为`%`,而不是`localhost`
+ 如果连接数据库失败的话,点击Next Step会出现error提示
4. 在`Zabbix server details`里Host和Port不用改,Name就写`zabbix`
5. `Congratulations! You have successfully installed Zabbix frontend.`说明配置成功
+ 本质上修改`/etc/zabbix/web/zabbix.conf.php`文件
6. 自动跳转到`http://47.100.185.187/zabbix/index.php`

### 1-7. 使用Web的Zabbix
#### 1-7-1. 登录
+ User:Admin
+ Password:zabbix
#### 1-7-2. 修改密码
+ 只要在password和password(once again)修改就行了
#### 1-7-3. 连接Zabbix的agentd端和server端
修改agentd端的`Server=47.100.185.187`,使Zabbix的agentd端连接到server端,并重启`sudo systemctl restart zabbix-agent`
#### 1-7-4. 启用Zabbix
在Configuration的Hosts里点击`Zabbix Server`,设置Agent Interfaces的ip地址为对应zabbix-agent服务器的ip地址,并将第1个Host的地方勾选`enabled`
#### 1-7-5. 配置服务器
1. 在`/etc/sysconfig/network`里配置`HOSTNAME=linux-node1.example.com`
2. 修改当前主机名:`hostname linux-node1.example.com`
3. 添加域名解析:`172.19.28.82 linux-node1.example.com  linux-node1`

#### 1-7-6. 在Zabbix里添加监控对象
1. 创建一个新的host group.名字就取demo,其他不用变
2. 添加主机
+ Host Name:linux-node1.example.com
+ Visible name:linux-node1
+ 添加到demo组里
+ Agent的IP地址改成agent服务器所在的地址
+ templates(上面导航栏点击)添加`Template OS Linux`(需要点击Add按钮)

#### 1-7-7. 查看监控对象的状态
1. Monitoring里面就是监控对象的状态
2. Monitoring>Graphs中,选好Groups,Host,Graph就能显示结果,比如说选择demo中的linux-node1主机的CPU使用率,就能查看到CPU的使用率情况



#### 1-7-0. 问题
#### 1-7-0-1. 使用`systemctl start zabbix-server`结果Web还是显示`zabbix-server is not running`
+ 查看端口发现zabbix-server没有运转
+ 查看错误日志(通过zabbix-server的配置文件查看log的位置),发现`connection to database 'zabbix' failed: [2002] Can't connect to local MySQL server through socket '/var/lib/mysql/mysql.sock' (2)`
##### 1-7-0-1-1. 解决
因为我编译MySQL时`mysql.sock`的位置在`/application/mysql-5.7.22/tmp/mysql.sock`里,所以应该添加软连接`sudo ln -s /application/mysql/tmp/mysql.sock /var/lib/mysql/mysql.sock`
+ 我本来想修改zabbix-server的配置文件,后来发现这样会很麻烦,所以直接用软连接了


## 2. 编译安装
### 2-1. 安装准备
```
sudo yum install -y net-snmp-devel
sudo yum install -y libssh2-devel
sudo yum install OpenIPMI-devel -y
sudo yum install libevent-devel -y
```
### 2-2. 正常操作
这里创建家目录其实等效于后续的`mkdir`+`chown`
```
tar -zxf zabbix-3.4.11.tar.gz 
cd zabbix-3.4.11
sudo groupadd --system zabbix
sudo useradd --system -g zabbix -d /usr/lib/zabbix -s /sbin/nologin -c "Zabbix Monitoring System" zabbix
./configure --help
sudo ./configure \
--enable-server \
--enable-agent \
--enable-proxy \
--with-mysql \
--enable-ipv6 \
--with-net-snmp \
--with-libcurl \
--with-libxml2 \
--with-ssh2 \
--with-openssl \
--with-openipmi 
sudo make install
```

### 2-0. 问题
#### 2-0-1. error: Invalid Net-SNMP directory - unable to find net-snmp-config
```
sudo yum install -y net-snmp-devel
```
#### 2-0-2. error: SSH2 library not found
```
sudo yum install -y libssh2-devel
```
#### 2-0-3. error: Unable to use libevent (libevent check failed)
```
sudo yum install OpenIPMI-devel -y
```

#### 2-0-4. error: Unable to use libevent (libevent check failed)
```
sudo yum install libevent-devel -y
```
