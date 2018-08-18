## 1. 安装
### 1-1. yum安装
启动Rabbitmq服务
rabbitmq-server启动服务后端口是5672
```
systemctl enable rabbitmq-server.service
systemctl start rabbitmq-server.service
systemctl status rabbitmq-server.service
```
添加消息队列的认证
消息队列肯定要有认证系统
```
netstat -lntup | grep 5672

rabbitmqctl add_user openstack openstack
rabbitmqctl set_permissions openstack ".*" ".*" ".*"
rabbitmq-plugins list
rabbitmq-plugins enable rabbitmq_management
systemctl restart rabbitmq-server.service
```
```
检查网络状态
netstat -lntup | grep 15672
```
### 1-2. 编译安装
```
########################################################################################
# 确认环境
########################################################################################
[root@rabbitmq ~]# cat /etc/redhat-release 
CentOS Linux release 7.5.1804 (Core) 
[root@rabbitmq ~]# uname -r
3.10.0-862.el7.x86_64
[root@rabbitmq ~]# uname -m
x86_64




########################################################################################
# 安装依赖包
########################################################################################
yum install -y wget make gcc gcc-c++ kernel-devel m4 ncurses-devel openssl-devel socat



########################################################################################
# 关闭防火墙,selinux
########################################################################################

systemctl stop iptables
systemctl disable iptables
systemctl stop firewalld
systemctl disable iptables
setenforce 0
sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config




########################################################################################
# 安装Erlang
########################################################################################
mkdir ~/tools
cd ~/tools

wget http://www.rabbitmq.com/releases/erlang/erlang-19.0.4-1.el7.centos.x86_64.rpm
rpm -ivh erlang-19.0.4-1.el7.centos.x86_64.rpm


########################################################################################
# 安装RabbitMQ
########################################################################################
cd ~/tools
wget http://www.rabbitmq.com/releases/rabbitmq-server/v3.6.15/rabbitmq-server-3.6.15-1.el7.noarch.rpm
rpm -ivh rabbitmq-server-3.6.15-1.el7.noarch.rpm



########################################################################################
# 启动服务
########################################################################################
rabbitmq-plugins enable rabbitmq_management
systemctl start rabbitmq-server

cp /usr/lib/rabbitmq/lib/rabbitmq_server-3.6.15/ebin/rabbit.app /usr/lib/rabbitmq/lib/rabbitmq_server-3.6.15/ebin/rabbit.app.bak
vi /usr/lib/rabbitmq/lib/rabbitmq_server-3.6.15/ebin/rabbit.app
#########################################################################################
{loopback_users, []},  # 原来是"{loopback_users, [<<"guest">>]},"的地方改成左边的
#########################################################################################



#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 访问127.0.0.1:15672,输入guest:guest,登录成功就说明OK
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
```

## 2. 操作
### 2-1. 用户
1. 添加
    1. Admin>Add Users,设置权限(tag)为administrator,并添加
    2. 命令行
    ```
    rabbitmqctl add_user orris orris
    rabbitmqctl set_permissions orris ".*" ".*" ".*"
    ```
### 2-2. 虚拟主机
1. 添加虚拟主机
    > Admin>右侧栏点击Virtual Hosts>Add a new virtual host >/orris
2. 给虚拟主机添加用户
    > 点击vhost,选择orris用户,并且选中这个orris这个host,然后里面
