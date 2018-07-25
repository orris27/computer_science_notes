## 1. 安装和配置
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
在Configuration的Hosts里点击`Zabbix Server`,将第1个Host的地方勾选`enabled`

#### 1-7-1. 使用`systemctl start zabbix-server`结果Web还是显示`zabbix-server is not running`
+ 查看端口发现zabbix-server没有运转
+ 查看错误日志(通过zabbix-server的配置文件查看log的位置),发现`connection to database 'zabbix' failed: [2002] Can't connect to local MySQL server through socket '/var/lib/mysql/mysql.sock' (2)`
##### 1-7-1-1. 解决
因为我编译MySQL时`mysql.sock`的位置在`/application/mysql-5.7.22/tmp/mysql.sock`里,所以应该添加软连接`sudo ln -s /application/mysql/tmp/mysql.sock /var/lib/mysql/mysql.sock`
+ 我本来想修改zabbix-server的配置文件,后来发现这样会很麻烦,所以直接用软连接了
