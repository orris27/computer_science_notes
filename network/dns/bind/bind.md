## 1. 安装
1. 安装软件包
2. 修改配置文件

```
############################################################
# 10.0.0..8 DNS主
############################################################




yum install bind-utils bind bind-devel bind-chroot -y
#rpm -qa | grep bind

vim /etc/named.conf
############################################################
options {
  version "1.1.1";
  listen-on port 53 { any; };
  directory  "/var/named/chroot/etc/";
  pid-file "/var/named/chroot/var/run/named/named.pid";
  allow-query     { any; };
  dump-file  "/var/named/chroot/var/log/binddump.db";
  statistics-file "/var/named/chroot/var/log/named_stats";
  zone-statistics yes;
  memstatistics-file "log/mem_stats";
  empty-zones-enable no;
  forwarders {202.106.196.115;8.8.8.8; };
  recursion yes;
  dnssec-enable yes;
  dnssec-validation yes;
  /* Path to ISC DLV key */
  bindkeys-file "/etc/named.iscdlv.key";
  managed-keys-directory "/var/named/dynamic";
  session-keyfile "/run/named/session.key";
};

key "rndc-key" {
  algorithm hmac-md5;
  secret "Eqw4hClGExUWeDkKBX/pBg==";
};

controls {
  inet 127.0.0.1 port 953
      allow { 127.0.0.1; } keys { "rndc-key"; };
};


logging {
  channel warning {
    file "/var/named/chroot/var/log/dns_warning" versions 10 size 10m;
    severity warning;
    print-category yes;
    print-severity yes;
    print-time yes;
  };
  channel general_dns {
    file "/var/named/chroot/var/log/dns_log" versions 10 size 100m;
    severity info;
    print-category yes;
    print-severity yes;
    print-time yes;
  }; 
  category default {
    warning;
  };
  category queries {
   general_dns;
  };
};

include "/var/named/chroot/etc/view.conf";
############################################################
# md5sum /etc/named.conf


vim /etc/rndc.key
############################################################
key "rndc-key" {
    algorithm hmac-md5;
    secret "Eqw4hClGExUWeDkKBX/pBg==";
};
############################################################


vim /etc/rndc.conf
# options表示用什么key去连接哪个服务器
############################################################
key "rndc-key" {
    algorithm hmac-md5;
    secret "Eqw4hClGExUWeDkKBX/pBg==";
};
options {
    default-key "rndc-key";
    default-server 127.0.0.1;
    default-port 953;

};
############################################################

vim /var/named/chroot/etc/view.conf
############################################################
view "View" {
    zone "lnh.com" {
        type master;
        file "lnh.com.zone";
        allow-transfer {
			10.0.0.7;
        };
        notify yes;
        also-notify {
			10.0.0.7;
        };
    };
};
############################################################

vim /var/named/chroot/etc/lnh.com.zone
############################################################
$ORIGIN .
$TTL 3600 ; 1hour
lnh.com      IN SOA op.lnh.com. dns.lnh.com. (
    2000    ; serial
    900     ; refresh (15 minutes)
    600     ; retry (10 minutes)
    86400   ; expire (1 day)
    3600    ; minimum (1 hour)
            ) 
          NS op.lnh.com.
$ORIGIN lnh.com.
shanks     A   1.2.3.4
op         A   1.2.3.4
############################################################


cd /var && chown -R named.named named/
systemctl start named
systemctl enable named



named-checkzone /etc/named.rfc1912.zones  /var/named/chroot/etc/lnh.com.zone

dig @127.0.0.1 shanks.lnh.com
# 1.2.3.4就说明部署成功了

```


## 2. 配置文件
1. `/etc/named.conf`:
    1. `version`:
    2. `listen-on`:
    3. `chroot`:bind提供的安全机制.提供一个虚拟的环境,root进来也不是真正的根
        1. directory:整个配置文件的位置
    4. `allow-query`:如果我开了53端口,那么只要你来访问我,我就给你回应
    5. `dump-file`:把一些内存的数据存进指定位置
        6. `sttistics-file`:保存状态数据
    7. `zone-statistics`: 是不是要把每个zone的状态都保存数据=>方便分析
    8. `memstatistics-file`: 内存状态的文件
    9. `forwarders`: DNS有递归和迭代两种方式.这里是迭代,表示如果我这里没有我就找他们
        + 如果我没有zone和缓存,就转发出去
        + 如果我有缓存,或者我有zone,就返回结果
            + 如果我有zone但是里面没有记录,就返回我自己的IP地址(我不太确定对不对?)
    10. `rndc-key`:允许哪些ip和认证可以对我进行reload操作
    11. `logging`:日志

2. `/var/named/chroot /etc/view.conf`
    1. `allow-transfer`:允许谁来向我这边请求数据
    2. `also-notify`:我更新数据了,同时去通知这些slaves
    3. `zone`:我应该去为哪个域解析.
    4. `file`:解析记录的存放位置

3. `/var/named/chroot/etc/lnh.com.zone`
    1. `ORIGIN`:如果配置`.`的话,声明我们的环境在`.`下用的
    2. `refresh`:每隔多久slave去更新数据
    3. `retry`:如果slave请求master数据失败,重试次数
    4. `expire`:多长时间内slave不能请求到master,那么slave就放弃master
    5. `NS`:nameserver域的名称
    6. `A`记录:NS的记录一定要放在A记录里,这样就能定位下一个DNS服务


## 3. 部署从DNS服务器
1. 配置文件基本相同.但view.conf不一致
	
```
##############################################################
# 10.0.0.7 DNS从服务器
##############################################################
yum install bind-utils bind bind-devel bind-chroot -y
#rpm -qa | grep bind

vim /etc/named.conf
############################################################
options {
  version "1.1.1";
  listen-on port 53 { any; };
  directory  "/var/named/chroot/etc/";
  pid-file "/var/named/chroot/var/run/named/named.pid";
  allow-query     { any; };
  dump-file  "/var/named/chroot/var/log/binddump.db";
  statistics-file "/var/named/chroot/var/log/named_stats";
  zone-statistics yes;
  memstatistics-file "log/mem_stats";
  empty-zones-enable no;
  forwarders {202.106.196.115;8.8.8.8; };
  recursion yes;
  dnssec-enable yes;
  dnssec-validation yes;
  /* Path to ISC DLV key */
  bindkeys-file "/etc/named.iscdlv.key";
  managed-keys-directory "/var/named/dynamic";
  session-keyfile "/run/named/session.key";
};

key "rndc-key" {
  algorithm hmac-md5;
  secret "Eqw4hClGExUWeDkKBX/pBg==";
};

controls {
  inet 127.0.0.1 port 953
      allow { 127.0.0.1; } keys { "rndc-key"; };
};


logging {
  channel warning {
    file "/var/named/chroot/var/log/dns_warning" versions 10 size 10m;
    severity warning;
    print-category yes;
    print-severity yes;
    print-time yes;
  };
  channel general_dns {
    file "/var/named/chroot/var/log/dns_log" versions 10 size 100m;
    severity info;
    print-category yes;
    print-severity yes;
    print-time yes;
  }; 
  category default {
    warning;
  };
  category queries {
   general_dns;
  };
};

include "/var/named/chroot/etc/view.conf";
############################################################
# md5sum /etc/named.conf


vim /etc/rndc.key
############################################################
key "rndc-key" {
    algorithm hmac-md5;
    secret "Eqw4hClGExUWeDkKBX/pBg==";
};
############################################################


vim /etc/rndc.conf
# options表示用什么key去连接哪个服务器
############################################################
key "rndc-key" {
    algorithm hmac-md5;
    secret "Eqw4hClGExUWeDkKBX/pBg==";
};
options {
    default-key "rndc-key";
    default-server 127.0.0.1;
    default-port 953;

};
############################################################

vim /var/named/chroot/etc/view.conf
# 修改下master的IP,如果多个master就用分号隔开.要保证master完全相同
############################################################
view "SlaveView" {
    zone "lnh.com" {
        type slave;
		master { 10.0.0.8; };
        file "slave.lnh.com.zone";
    };
};
############################################################



##############################################################
# 10.0.0.8 DNS主服务器
##############################################################
vim /var/named/chroot/etc/lnh.com.zone
############################################################
$ORIGIN .
$TTL 3600 ; 1hour
lnh.com      IN SOA op.lnh.com. dns.lnh.com. (
    2001    ; serial
    900     ; refresh (15 minutes)
    600     ; retry (10 minutes)
    86400   ; expire (1 day)
    3600    ; minimum (1 hour)
            ) 
          NS op.lnh.com.
$ORIGIN lnh.com.
shanks     A   1.2.3.4
op         A   1.2.3.4
a          A   10.0.0.100 ;添加了新的A记录后,要增大serial值,原来是2000,现在变成了2001
############################################################
rndc reload








##############################################################
# 10.0.0.7 DNS从服务器
##############################################################


cd /var && chown -R named.named named/
systemctl start named
systemctl enable named


ll /var/named/chroot/etc #如果出现slave.lnh.com.zone,就说明正常了.因为这个是master同步过来的



##############################################################
# 10.0.0.8 DNS主服务器
##############################################################
dig @10.0.0.8 a.lnh.com 
dig @10.0.0.7 a.lnh.com # 如果都出现10.0.0.100,就说明解析成功
```


## 4. 测试
```
##############################################################
# 10.0.0.8 DNS主服务器
##############################################################

vim /var/named/chroot/etc/lnh.com.zone
############################################################
$ORIGIN .
$TTL 3600 ; 1hour
lnh.com      IN SOA op.lnh.com. dns.lnh.com. (
    2002    ; serial
    900     ; refresh (15 minutes)
    600     ; retry (10 minutes)
    86400   ; expire (1 day)
    3600    ; minimum (1 hour)
            ) 
          NS op.lnh.com.
$ORIGIN lnh.com.
shanks     A   1.2.3.4
op         A   1.2.3.4
a          A   10.0.0.100 
a          A   192.168.122.100 ;2001=>2002
############################################################
rndc reload

host a.lnh.com 127.0.0.1 # 使用DNS主服务器测试解析a.lnh.com的结果
host a.lnh.com 10.0.0.7 # 使用DNS从服务器测试解析a.lnh.com的结果
# 如果都出现10.0.0.100 和 192.168.122.100,就说明正确








vim /var/named/chroot/etc/lnh.com.zone
# serial +1 是为了能让解析同步到slave上
############################################################
$ORIGIN .
$TTL 3600 ; 1hour
lnh.com      IN SOA op.lnh.com. dns.lnh.com. (
    2003    ; serial
    900     ; refresh (15 minutes)
    600     ; retry (10 minutes)
    86400   ; expire (1 day)
    3600    ; minimum (1 hour)
            ) 
          NS op.lnh.com.
$ORIGIN lnh.com.
shanks     A   1.2.3.4
op         A   1.2.3.4
a          A   10.0.0.100 
a          A   192.168.122.100 
cname      CNAME a.lnh.com.      ;2002=>2003
############################################################
rndc reload

host cname.lnh.com 127.0.0.1
host cname.lnh.com 10.0.0.7
# 如果都出现10.0.0.100 和 192.168.122.100,就说明正确
# cname.lnh.com is an alias for a.lnh.com




vim /var/named/chroot/etc/lnh.com.zone
# MX加2条时, serial只+1
############################################################
$ORIGIN .
$TTL 3600 ; 1hour
lnh.com      IN SOA op.lnh.com. dns.lnh.com. (
    2004    ; serial
    900     ; refresh (15 minutes)
    600     ; retry (10 minutes)
    86400   ; expire (1 day)
    3600    ; minimum (1 hour)
            ) 
          NS op.lnh.com.
$ORIGIN lnh.com.
shanks     A   1.2.3.4
op         A   1.2.3.4
a          A   10.0.0.100 
a          A   192.168.122.100 
cname      CNAME a.lnh.com.      
mx         MX 5  192.168.122.101  
mx         MX 10 192.168.123.101 ;2003=>2004
############################################################
rndc reload

host mx.lnh.com 127.0.0.1
host mx.lnh.com 10.0.0.7
# 如果都出现下面的,说明就好了
#mx.lnh.com mail is handled by 5 192.168.122.101.lnh.com.
#mx.lnh.com mail is handled by 5 192.168.123.101.lnh.com.


```
反向解析(PTR)
+ `zone "168.192.in-addr.arpa"`:表示`192.168`这个网段,而这个网段的PTR名称规定就是这样的
    1. 在master上添加新的zone
    2. 配置zone文件,并重启rndc
    3. 告诉slave要同步新的zone文件,并重启rndc
```
##############################################################
# 10.0.0.8 DNS主服务器
##############################################################

vim /var/named/chroot/etc/view.conf
############################################################
view "View" {
    zone "lnh.com" {
        type master;
        file "lnh.com.zone";
        allow-transfer {
		10.0.0.7;
        };
        notify yes;
        also-notify {
		10.0.0.7;
        };
    };
    zone "168.192.in-addr.arpa" {
        type master;
        file "168.192.zone";
        allow-transfer {
		10.0.0.7;
        };
        notify yes;
        also-notify {
		10.0.0.7;
        };
    };
};
############################################################



vim /var/named/chroot/etc/168.192.zone
# 因为是新添加的zone文件,所以serial可以不用更改
############################################################
$TTL 3600 ; 1hour
@      IN SOA op.lnh.com. dns.lnh.com. (
    2004    ; serial
    900     ; refresh (15 minutes)
    600     ; retry (10 minutes)
    86400   ; expire (1 day)
    3600    ; minimum (1 hour)
            ) 
		     NS op.lnh.com.
102.100         IN       PTR       a.lnh.com.
############################################################
chown named.named /var/named/chroot/etc/168.192.zone
rndc reload # 一定要注意






##############################################################
# 10.0.0.7 DNS从服务器
##############################################################
vim /var/named/chroot/etc/view.conf
############################################################
view "SlaveView" {
    zone "lnh.com" {
        type slave;
        master { 10.0.0.8; };
        file "slave.lnh.com.zone";
    };
    zone "168.192.in-addr.arpa" {
        type slave;
        master { 10.0.0.8; };
        file "slave.168.192.zone";
    };
};
############################################################
rndc reload

host 192.168.122.102 127.0.0.1
host 192.168.122.102 10.0.0.8
#-----------------------------------------------------------
# 102.122.168.192.in-addr.arpa domain name pointer a.lnh.com.
#-----------------------------------------------------------
```

通过DNS实现服务的负载均衡
+ 高效粗暴
+ 只能轮询
+ 没有健康检测
```
##############################################################
# 10.0.0.8 DNS主服务器
##############################################################
vim /var/named/chroot/etc/lnh.com.zone
# 建议同样类型的域名放一起
############################################################
$ORIGIN .
$TTL 3600 ; 1hour
lnh.com      IN SOA op.lnh.com. dns.lnh.com. (
    2005    ; serial
    900     ; refresh (15 minutes)
    600     ; retry (10 minutes)
    86400   ; expire (1 day)
    3600    ; minimum (1 hour)
            ) 
          NS op.lnh.com.
$ORIGIN lnh.com.
shanks     A   1.2.3.4
op         A   1.2.3.4
a          A   10.0.0.100 
a          A   192.168.122.100
a          A   192.168.122.102 ;2004=>2005

cname      CNAME a.lnh.com.      
mx         MX 5  192.168.122.101  
mx         MX 10 192.168.123.101 
############################################################
rndc reload


host a.lnh.com 127.0.0.1
host a.lnh.com 10.0.0.7
# 出现三个就行
```

配置DNS视图(智能DNS)
1. acl
    1. 在组里配置了acl的名称和对应的ip池
    2. 如果IP在ip池α里面,那么对应的acl组名就是对应的
        + 比如10.0.0.7去访问的话,就是group2(group2名字随意命名)
2. 原理
    > 通过不同IP请求,分配到不同ACL组里,然后ACL组各自对应一个zone文件,用各自的zone文件来处理这个IP的DNS请求
```
##############################################################
# 10.0.0.8 DNS主服务器
##############################################################
vim /var/named/chroot/etc/namd.conf
############################################################
options {
  version "1.1.1";
  listen-on port 53 { any; };
  directory  "/var/named/chroot/etc/";
  pid-file "/var/named/chroot/var/run/named/named.pid";
  allow-query     { any; };
  dump-file  "/var/named/chroot/var/log/binddump.db";
  statistics-file "/var/named/chroot/var/log/named_stats";
  zone-statistics yes;
  memstatistics-file "log/mem_stats";
  empty-zones-enable no;
  forwarders {202.106.196.115;8.8.8.8; };
  recursion yes;
  dnssec-enable yes;
  dnssec-validation yes;
  /* Path to ISC DLV key */
  bindkeys-file "/etc/named.iscdlv.key";
  managed-keys-directory "/var/named/dynamic";
  session-keyfile "/run/named/session.key";
};

key "rndc-key" {
  algorithm hmac-md5;
  secret "Eqw4hClGExUWeDkKBX/pBg==";
};

controls {
  inet 127.0.0.1 port 953
      allow { 127.0.0.1; } keys { "rndc-key"; };
};


logging {
  channel warning {
    file "/var/named/chroot/var/log/dns_warning" versions 10 size 10m;
    severity warning;
    print-category yes;
    print-severity yes;
    print-time yes;
  };
  channel general_dns {
    file "/var/named/chroot/var/log/dns_log" versions 10 size 100m;
    severity info;
    print-category yes;
    print-severity yes;
    print-time yes;
  }; 
  category default {
    warning;
  };
  category queries {
   general_dns;
  };
};

// 只要编辑这下面的内容就可以了!!!
acl group1 {
    10.0.0.7;
};
acl group2 {
    10.0.0.8;
};


include "/var/named/chroot/etc/view.conf";
############################################################



vim /var/named/chroot/etc/view.conf
# view的名字和acl里的名字不需要相同
# 真正调用的是match_clients里面的,里面的名字要和acl的名字相同
############################################################
view "GROUP1" {
    match-clients { group1; };
    zone "viewlnh.com" {
        type master;
        file "group1.viewlnh.com.zone";
    };
};

view "GROUP2" {
    match-clients { group2; };
    zone "viewlnh.com" {
        type master;
        file "group2.viewlnh.com.zone";
    };
};
############################################################

vim /var/named/chroot/etc/group1.viewlnh.com.zone
############################################################
$ORIGIN .
$TTL 3600 ; 1hour
viewlnh.com      IN SOA op.viewlnh.com. dns.viewlnh.com. (
    2005    ; serial
    900     ; refresh (15 minutes)
    600     ; retry (10 minutes)
    86400   ; expire (1 day)
    3600    ; minimum (1 hour)
            ) 
          NS op.viewlnh.com.
$ORIGIN viewlnh.com.
op           A   192.168.122.1
view         A   192.168.122.1
############################################################


vim /var/named/chroot/etc/group2.viewlnh.com.zone
############################################################
$ORIGIN .
$TTL 3600 ; 1hour
viewlnh.com      IN SOA op.viewlnh.com. dns.viewlnh.com. (
    2005    ; serial
    900     ; refresh (15 minutes)
    600     ; retry (10 minutes)
    86400   ; expire (1 day)
    3600    ; minimum (1 hour)
            ) 
          NS op.viewlnh.com.
$ORIGIN viewlnh.com.
op           A   192.168.122.2
view         A   192.168.122.2
############################################################
chown named.named /var/named/chroot/etc/group1.viewlnh.com.zone
chown named.named /var/named/chroot/etc/group2.viewlnh.com.zone
rndc reload


##############################################################
# 10.0.0.7 DNS从服务器
##############################################################

dig @10.0.0.7 view.viewlnh.com 
dig @10.0.0.8 view.viewlnh.com 
# 出现192.168.122.1 就说明正确




##############################################################
# 10.0.0.8 DNS主服务器
##############################################################
dig @10.0.0.7 view.viewlnh.com 
dig @10.0.0.8 view.viewlnh.com 
# 出现192.168.122.2 就说明正确

```
