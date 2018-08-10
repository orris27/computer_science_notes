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

