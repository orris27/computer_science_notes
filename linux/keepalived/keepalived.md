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


## 2. 配置Keepalived
### 2-1. 单实例
假设有2台服务器,分别记作A(`10.0.0.7`)和B(`10.0.0.9`).在2台服务器上都配置LVS和Keepalived.然后A为master而B为backup.
1. 在A和B上安装LVS,安装到`lsmod | grep ip_vs`出现结果就行了
2. 在A和B上安装Keepalived,安装到服务能成功启动为止
3. 将A和B多余的网络接口给取消掉,如`eth0:0`和`lo:0`
    + `ifconfig xx down`
4. 在A上配置Keepalived的单实例master配置文件
5. 在B上配置Keepalived的单实例backup配置文件
6. 关闭A和B的防火墙和selinux
    + 防止裂脑
7. 重启Keepalived服务
8. 验证
```
########################################################
# 10.0.0.7 & 10.0.0.9
########################################################
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
# root       9209  0.0  0.1  695r20  1608 ?        S    14:45   0:00 /usr/local/sbin/keepalived -D
# root       9236  0.0  0.0 112704   972 pts/0    S+   14:46   0:00 grep --color=auto keepalived
#-------------------------------------------------------------------








########################################################
# 10.0.0.7
########################################################
ifconfig eth0:0 down # 取消掉无用的网络接口

### 2-1. 单实例
cp /etc/keepalived/keepalived.conf /etc/keepalived/keepalived.conf.bak

cat > /etc/keepalived/keepalived.conf <<EOF
global_defs {
    notification_email {
        xxx@qq.com
    }
    notification_email_from Alexandre.Cassen@firewall.loc
    smtp_server 10.0.0.1
    smtp_connect_timeout 30
    router_id LVS_7
}

vrrp_instance VI_1 {
    state MASTER
    interface eth0
    virtual_router_id 55
    priority 150
    advert_int 1
    authentication {
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress {
        10.0.0.10/24
    }
}
EOF

systemctl stop iptables
systemctl disable iptables
systemctl stop firewalld
systemctl disable firewalld
setenfore 0
systemctl restart keepalived
ip add # 看是否有10.0.0.10,应该要有才行
# 如果有10.0.0.10说明现在是









########################################################
# 10.0.0.9
########################################################
ifconfig eth0:0 down

cp /etc/keepalived/keepalived.conf /etc/keepalived/keepalived.conf.bak

cat > /etc/keepalived/keepalived.conf <<EOF
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
    interface eth0
    virtual_router_id 55
    priority 100
    advert_int 1
    authentication {
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress {
        10.0.0.10/24
    }
}
EOF

ifconfig lo:0 down
systemctl stop iptables
systemctl disable iptables
systemctl stop iptables
systemctl disable firewalld
setenfore 0

systemctl restart keepalived

# 通过keepalived,ip地址看不到
ip add # 看是否有10.0.0.10,应该要没有才行=>因为现在10.0.0.9只是BACKUP,而不是MASTER


# => 现在只是实现了VIP地址的接管.
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 我们可以不断ping这个VIP,然后kill掉一个LVS,看是不是还能ping通
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
```
### 2-1. 多实例
服务器A(`10.0.0.7`)和B(`10.0.0.9`)上跑
1. 实例1
    1. 服务1
    2. A=>B
    3. `vrrp_instance`:VI_1
    4. VIP:`10.0.0.130`
2. 实例2
    1. 服务2
    2. B=>A
    3. `vrrp_instance`:VI_1
    4. VIP:`10.0.0.140`
```
###########################################################################
# 10.0.0.7
###########################################################################
cp /etc/keepalived/keepalived.conf /etc/keepalived/keepalived.conf.bak

cat > /etc/keepalived/keepalived.conf <<EOF
global_defs {
    notification_email {
        xxx@qq.com
    }
    notification_email_from Alexandre.Cassen@firewall.loc
    smtp_server 10.0.0.1
    smtp_connect_timeout 30
    router_id LVS_7
}

vrrp_instance VI_1 {
    state BACKUP
    interface eth0
    virtual_router_id 51
    priority 100
    advert_int 1
    authentication {
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress {
        10.0.0.130/24
    }
}
vrrp_instance VI_2 {
    state MASTER
    interface eth0
    virtual_router_id 52
    priority 150
    advert_int 1
    authentication {
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress {
        10.0.0.140/24
    }
}
EOF
ifconfig lo:0 down
systemctl stop iptables
systemctl disable iptables
systemctl stop iptables
systemctl disable firewalld
setenfore 0

systemctl restart keepalived

ip add 




###########################################################################
# 10.0.0.9
###########################################################################
cp /etc/keepalived/keepalived.conf /etc/keepalived/keepalived.conf.bak

cat > /etc/keepalived/keepalived.conf <<EOF
global_defs {
    notification_email {
        xxx@qq.com
    }
    notification_email_from Alexandre.Cassen@firewall.loc
    smtp_server 10.0.0.1
    smtp_connect_timeout 30
    router_id LVS_7
}

vrrp_instance VI_1 {
    state MASTER
    interface eth0
    virtual_router_id 51
    priority 150
    advert_int 1
    authentication {
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress {
        10.0.0.130/24
    }
}
vrrp_instance VI_2 {
    state BACKUP
    interface eth0
    virtual_router_id 52
    priority 100
    advert_int 1
    authentication {
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress {
        10.0.0.140/24
    }
}
EOF
systemctl stop iptables
systemctl disable iptables
systemctl stop firewalld
systemctl disable firewalld
setenfore 0
systemctl restart keepalived
ip add 

```


## 3. Keepalived接管Web
如果A和B都配置好Web和Keepalived,不需要配置Web和Keepalived的联系
    > 原因: 当我们curl VIP的时候,会自动找到拥有VIP的主机,然后就找到了这个主机,这样就是curl 这个主机了(我们本质也是通过VIP找到主机)


## 4. 修改日志存放文件
Keepalived默认的日志存放文件是`/var/log/messages`
1. 告诉Keepalived我们的日志存放文件是设备0
2. 告诉系统,日志设备0对应的文件是`/var/log/keepalived.log`
    + 系统的syslog有8个级别,分别是0-8
3. 重启rsyslog和keepalived
4. 好像没有生效...
```

sed  -i 's/KEEPALIVED_OPTIONS="-D"/#KEEPALIVED_OPTIONS="-D"\nKEEPALIVED_OPTIONS="-D -d -S 0"/' /etc/sysconfig/keepalived 
############################################################################
#KEEPALIVED_OPTIONS="-D"
KEEPALIVED_OPTIONS="-D -d -S 0"
############################################################################


cp /etc/rsyslog.conf /etc/rsyslog.conf.bak
echo "local0.*                                                /var/log/keepalived.log" >>/etc/rsyslog.conf
############################################################################
# Save boot messages also to boot.log
local7.*                                                /var/log/boot.log
local0.*                                                /var/log/keepalived.log
############################################################################

systemctl restart rsyslog
systemctl restart keepalived

```



## 5.配置LVS+keepalived
Keepalived不通过ipvsadm来管理LVS,而是有自己的接口
1. 介绍
    1. virtual_server<=>添加主机
    2. 默认用端口检查
    3. Keepalived自动实现健康检查
        + 健康检查有`TCP_CHECK`等方法
    4. Keepalived的语法检查比较辣鸡,要自己检查
    5. 配置好后,只要启动Keepalived后,自动就会生成ipvsadm的表
2. 使用
    1. 修改配置文件
        1. 主要修改1个`virtual_server`和2个`real_server`
    2. 下面的配置文件追加到Keepalived的配置文件里面
    3. real server绑定VIP和抑制ARP,LVS上用eth0绑定VIP(如果有个服务器既是LVS又是real server,用eth0:0还是lo:0绑定VIP我不知道.最好还是不要这样设计冲突的架构)
    4. NAT模式打开内核参数的`net.ipv4.ip_forward = 1`
    5. 重启Keepalived服务
```
# virtual_server
#ipvsadm --add-service --tcp-service 10.0.0.29:80 --scheduler wrr --persistent 20


cat >> /etc/keepalived/keepalived.conf <<EOF
virtual_server 10.0.0.10 80{
    delay_loop 6
    lb_algo wrr
    lb_kind DR
    nat_mask 255.255.255.0
    persistence_timeout 300
    protocol TCP
    real_server 10.0.0.8 80 {
        weight 1
        TCP_CHECK {
            connect_timeout 8
            nb_get_retry 8
            delay_before_retry 3
            connect_port 80
        }
    }
    real_server 10.0.0.9 80 {
        weight 1
        TCP_CHECK {
            connect_timeout 8
            nb_get_retry 3
            delay_before_retry 3
            connect_port 80
        }
    }
}
EOF

ifconfig eth0:0 10.0.0.10/24 up # LVS01

ifconfig lo:0 10.0.0.10/32 up # Real servers
echo "1" > /proc/sys/net/ipv4/conf/lo/arp_ignore
echo "2" > /proc/sys/net/ipv4/conf/lo/arp_announce
echo "1" > /proc/sys/net/ipv4/conf/all/arp_ignore
echo "2" > /proc/sys/net/ipv4/conf/all/arp_announce


echo "net.ipv4.ip_forward = 1" >> /etc/sysctl.conf
sysctl -p

systemctl restart keepalived


```



## 6. 配置文件
1. 单实例
    1. Master的配置文件.
        + VIP是`10.0.0.10/24`
    ```
    vim /etc/keepalived/keepalived.conf
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
        interface eth0 ! 内网的接口
        virtual_router_id 55 ! 虚拟的路由ID
        priority 150 ! Keepalived通过竞选机制来选取备的,这里就是优先级
        advert_int 1 ! 高可用间监控的时间监控.1秒
        authentication { ! Keepalived之间用这个来认证
            auth_type PASS
            auth_pass 1111
        }
        virtual_ipaddress { ! VIP,一定要给24
            10.0.0.10/24
        }
    }
    ###########################################################################
    ```
    2. Backup的配置文件.
        + VIP是`10.0.0.10/24`    
    ```
    vim /etc/keepalived/keepalived.conf
    ###########################################################################
    global_defs {
        notification_email {
            xxx@qq.com
        }
        notification_email_from Alexandre.Cassen@firewall.loc
        smtp_server 10.0.0.1
        smtp_connect_timeout 30
        router_id LVS_2 ! 不一样的路由ID
    }

    vrrp_instance VI_1 {
        state BACKUP ! 这里也不一样!
        interface eth0
        virtual_route_id 55 ! 虚拟的路由ID.相同的实例要一样
        priority 100 ! 官方推荐差50
        advert_int 1
        authentication {
            auth_type PASS
            auth_pass 1111
        }
        virtual_ipaddress {
            10.0.0.10/24
        }
    }
    ###########################################################################

    ```



2. 多实例
    1. 注意
        1. 实例不同
            1. `vrrp_instance`不同
            2. `virtual_router_ip`不同
            3. VIP不同
    2. 配置
        2个服务器每个上面上跑2个Keepalived来维护2个不同的服务α和β.我们这里仍然选择A(`10.0.0.7`)和B(`10.0.0.9`).A和B都跑2个Keepalived,用vrrp_instance标记不同服务(实例),即`VI_1`和`VI_2`,并且使用的虚拟VIP自然也各不相同.α:A=>B.β:B=>A
        1. 在A上配置多实例配置文件.
            1. 实例α上为Backup
            2. 实例β上为Master
        ```
        vim /etc/keepalived/keepalived.conf
        ###########################################################################
        global_defs {
            notification_email {
                xxx@qq.com
            }
            notification_email_from Alexandre.Cassen@firewall.loc
            smtp_server 10.0.0.1
            smtp_connect_timeout 30
            router_id LVS_7
        }

        vrrp_instance VI_1 {
            state BACKUP
            interface eth0
            virtual_router_id 51
            priority 100
            advert_int 1
            authentication {
                auth_type PASS
                auth_pass 1111
            }
            virtual_ipaddress {
                10.0.0.130/24
            }
        }
        vrrp_instance VI_2 {
            state MASTER
            interface eth0
            virtual_router_id 52
            priority 150
            advert_int 1
            authentication {
                auth_type PASS
                auth_pass 1111
            }
            virtual_ipaddress {
                10.0.0.140/24
            }
        }
        ###########################################################################
        ```
        
        2. 在B上配置多实例配置文件.
            1. 实例α上为Master
            2. 实例β上为Backup

        ```
        vim /etc/keepalived/keepalived.conf
        ###########################################################################
        global_defs {
            notification_email {
                xxx@qq.com
            }
            notification_email_from Alexandre.Cassen@firewall.loc
            smtp_server 10.0.0.1
            smtp_connect_timeout 30
            router_id LVS_7
        }

        vrrp_instance VI_1 {
            state MASTER
            interface eth0
            virtual_router_id 51
            priority 150
            advert_int 1
            authentication {
                auth_type PASS
                auth_pass 1111
            }
            virtual_ipaddress {
                10.0.0.130/24
            }
        }
        vrrp_instance VI_2 {
            state BACKUP
            interface eth0
            virtual_router_id 52
            priority 100
            advert_int 1
            authentication {
                auth_type PASS
                auth_pass 1111
            }
            virtual_ipaddress {
                10.0.0.140/24
            }
        }
        ###########################################################################
        ```


