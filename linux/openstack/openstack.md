## 1. 安装
1. 环境准备
    1. CentOS-7.1 系统2台,每台2G内存
    + linux-node1.oldboyedu.com 192.168.56.11 网卡NAT eth0 控制节点
    + linux-node2.oldboyedu.com 192.168.56.12 网卡NAT eth0 计算节点
        1. NAT实现外网+与物理机不同网段的实现
        + vment8       != 菜单栏的网卡   = 虚拟机的网卡  != 虚拟机的IP地址
        + 192.168.56.1 != 192.168.56.2 =192.168.56.2 != 192.168.56.11        
        > https://github.com/orris27/orris/blob/master/linux/vmware/installation.md
        2. 改变网卡名称
        > https://github.com/orris27/orris/blob/master/network/network.md
        
    
    2. 域名解析
    + OpenStack主机名很重要
    ```
    cat >> /etc/hosts <<EOF
    192.168.56.11 linux-node1 linux-node1.oldboyedu.com
    192.168.56.12 linux-node2 linux-node2.oldboyedu.com
    EOF
    ```
    3. 硬盘50G
    
    4. Tightvnc view VNC客户端
    
    5. 安装包:老师发给大家
    
    6. 理解SOA,RestAPI,消息队列,对象存储
    
    7. 使用L版OpenStack


2. 同步时间
```
##################################
# 控制节点
##################################
yum install chrony -y
vim /etc/chrony.conf
####################
allow  192.168/16 # 取消注释就行了
####################
systemctl enable chronyd.service
systemctl start chronyd.service

timedatectl set-timezone Asia/Shanghai
date # 和当前的时间一样
```

3. 安装数据库
+ yum源可以考虑使用阿里的源,执行`yum install -y wget;mv /etc/yum.repos.d/CentOS-Base.repo /etc/yum.repos.d/CentOS-Base.repo.bak;wget -O /etc/yum.repos.d/CentOS-Base.repo http://mirrors.aliyun.com/repo/Centos-7.repo`
```
yum install -y mariadb mariadb-server MySQL-python
cp /usr/share/mysql/my-medium.cnf /etc/my.cnf
vim /etc/my.cnf
######################################## 下面的内容添加到/etc/my.cnf的[mysqld]下面
default-storage-engine = innodb
innodb_file_per_table
collation-server = utf8_general_ci
init-connect = 'SET NAMES utf8'
character-set-server = utf8
########################################
systemctl enable mariadb.service
systemctl start mariadb.service # 启动的服务还是叫mysqld

mysql_secure_installation
# 设置当前密码:Y
# 输入密码
# 移除匿名用户:Y
# 关闭root远程登录:Y
# 删除默认的test数据:Y
# 刷新权限:Y
mysql -u root -p 
```
