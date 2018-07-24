## 1. 安装
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
