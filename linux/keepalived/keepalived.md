## 1. 安装

```
########################################################################
# 直接复制粘贴能成功!
########################################################################
yum install -y openssl*
yum install -y libnfnetlink-devel


sudo ln -s /usr/src/kernels/`uname -r`/ /usr/src/linux

mkdir ~/tools
cd ~/tools

wget http://www.keepalived.org/software/keepalived-2.0.6.tar.gz
tar zxf keepalived-2.0.6.tar.gz
cd keepalived-2.0.6
./configure

################################################################确保有3个YES
# Use IPVS Framework       : Yes # 框架要YES,这样就可以和LVS同步
# IPVS sync daemon support : Yes # 我这里没有这个属性,但我还是继续执行了
# Use VRRP Framework       : Yes
################################################################
make && make install 

/bin/cp /usr/local/etc/sysconfig/keepalived /etc/sysconfig/
mkdir /etc/keepalived -p
/bin/cp /usr/local/etc/keepalived/keepalived.conf /etc/keepalived/
/bin/cp /usr/local/sbin/keepalived /usr/sbin/

systemctl start keepalived

ps -ef | grep keep # 有3个keepalivd就是成功
#-------------------------------------------------------------------
# [root@vls01 init.d]# ps aux | grep keepalived
# root       9207  0.0  0.1  69520  1020 ?        Ss   14:45   0:00 /usr/local/sbin/keepalived -D
# root       9208  0.0  0.2  69652  2128 ?        S    14:45   0:00 /usr/local/sbin/keepalived -D
# root       9209  0.0  0.1  69520  1608 ?        S    14:45   0:00 /usr/local/sbin/keepalived -D
# root       9236  0.0  0.0 112704   972 pts/0    S+   14:46   0:00 grep --color=auto keepalived
#-------------------------------------------------------------------
```

## 2. 单实例
```
###########################################################################
global_defs {
    notification_email {
        xxx@qq.com
    }
    notification_email_from Alexandre.Cassen@firewall.loc
    smtp_server 10.0.0.1
    smtp_connect_timeout 30 ! 以上都可以删掉不选
    router_id LVS_7 ! 相当于MySQL的id
}

vrrp_instance VI_1 { ! 虚拟的路由实例
    state MASTER
    interface_eth0 ! 内网的接口
    virtual_route_id 55 ! 虚拟的路由ID
    priority 150 ! Keepalived通过竞选机制来选取备的,这里就是优先级
    advert_int 1 ! 高可用间监控的时间监控.1秒
    authentication { ! Keepalived之间用这个来认证
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress { # VIP,一定要给24
        10.0.0.9/24
    }
}
###########################################################################

###########################################################################
global_defs {
    notification_email {
        xxx@qq.com
    }
    notification_email_from Alexandre.Cassen@firewall.loc
    smtp_server 10.0.0.1
    smtp_connect_timeout 30
    router_id LVS_2
}

vrrp_instance VI_1 {
    state BACKUP
    interface_eth0
    virtual_route_id 55 ! 虚拟的路由ID.相同的实例要一样
    priority 100 ! 官方推荐差50
    advert_int 1
    authentication {
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress {
        10.0.0.9/24
    }
}
###########################################################################

```


## 3. 给10.0.0.9配置LVS
1. 在`10.0.0.9`上安装LVS,安装到`lsmod | grep ip_vs`出现结果就行了
```
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 安装LVS
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
lsmod | grep ip_vs
#----------------------------------------------------------
ip_vs                 141432  0 
nf_conntrack          133053  1 ip_vs
libcrc32c              12644  3 xfs,ip_vs,nf_conntrack
#----------------------------------------------------------

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 安装LVS
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
ps -ef | grep keep # 有3个keepalivd就是成功
#-------------------------------------------------------------------
# [root@vls01 init.d]# ps aux | grep keepalived
# root       9207  0.0  0.1  69520  1020 ?        Ss   14:45   0:00 /usr/local/sbin/keepalived -D
# root       9208  0.0  0.2  69652  2128 ?        S    14:45   0:00 /usr/local/sbin/keepalived -D
# root       9209  0.0  0.1  69520  1608 ?        S    14:45   0:00 /usr/local/sbin/keepalived -D
# root       9236  0.0  0.0 112704   972 pts/0    S+   14:46   0:00 grep --color=auto keepalived
#-------------------------------------------------------------------

```

