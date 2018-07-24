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
