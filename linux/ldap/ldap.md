    
## 1. 安装
1. 环境准备
    1. 一台主机
        1. eth0:192.168.1.7
        2. 桥接

```
#################################################################################
# 确认系统环境
#################################################################################
[root@ldap-server ~]# cat /etc/redhat-release 
CentOS Linux release 7.5.1804 (Core) 
[root@ldap-server ~]# uname -r
3.10.0-862.el7.x86_64
[root@ldap-server ~]# uname -m
x86_64



##################################################################################
# 关闭防火墙和SeLinux
##################################################################################
systemctl stop iptables
systemctl disable iptables
systemctl stop firewalld
systemctl disable firewalld
setenforce 0
sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config



#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 时间同步(参考https://github.com/orris27/orris/blob/master/linux/storage/glusterfs/glusterfs.md)
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



echo "192.168.1.7 etiantian.org" >> /etc/hosts
ping etiantian.org

# 功能性软件用yum简单,性能或定制的软件用编译
# --skip-broken可能需要加,也可能不需要加
yum install -y openldap openldap-* --skip-broken
yum install -y nscd nss-pam-ldapd nss-* pcre pcre-* --skip-broken

rpm -qa | grep openldap
#--------------------------------------------------------------------
# openldap-servers-sql-2.4.44-15.el7_5.x86_64
# openldap-2.4.44-15.el7_5.x86_64
# openldap-servers-2.4.44-15.el7_5.x86_64
# openldap-devel-2.4.44-15.el7_5.x86_64
# openldap-clients-2.4.44-15.el7_5.x86_64
#--------------------------------------------------------------------



cd /etc/openldap/
# cn=config才是配置文件
cp /usr/share/openldap-servers/slapd.ldif slapd.conf
# 生成ldap管理员密码
slappasswd --help
slappasswd -s orris
#--------------------------------------------------------------------
# {SSHA}kizFk+ZB7dcM8kR8LQPFpEz5KK3utE44
#--------------------------------------------------------------------
# orris就是管理员了
slappasswd -s orris | sed -e "s/{SSHA}/rootpw\ {SSHA}/g" >> slapd.conf 
tail -1 slapd.conf 
#--------------------------------------------------------------------
# rootpw {SSHA}/KruS38cDDHy7mIx5vIUQx0DF6Vt80rf
#--------------------------------------------------------------------
# 生成ldap的密码并且在配置文件里配置
cp slapd.conf slapd.conf.bak


```
