## 1. 安装
1. 安装软件包
2. 修改配置文件

```
yum install bind-utils bind bind-devel bind-chroot -y
#rpm -qa | grep bind

vim /etc/named.conf
############################################################
options {
  versions "1.1.1";
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
  channel general_nds {
    file "/var/named/chroot/var/log/dns" versions 10 size 100m;
    severity info;
    print-category yes;
    print-severity yes;
    print-time yes;
  }; 
  channel default {
    warning;
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
            10.255.253.211;
        };
        notify yes;
        also-notify {
            10.255.253.211;
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
