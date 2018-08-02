## 1. CentOS7.x的Kickstart配置文件
1. 有注释
```
# Kickstart Configurator for CentOS 7 by yao zhang #命令段

install #告知安装程序，这是一次全新安装，而不是升级

url --url="http://172.16.1.201/CentOS7/" #通过http下载安装镜像

text #以文本格式安装

lang en_US.UTF-8 #设置字符集格式

keyboard us #设置键盘类型

zerombr #清除mbr引导（清空引导扇区）

bootloader --location=mbr --driveorder=sda --append="crashkernel=auto rhgb quiet" #指定引导记录被写入的位置

network --bootproto=static --device=eth0 --gateway=10.0.0.254 --ip=10.0.0.63 --nameserver=223.5.5.5 --netmask=255.255.255.0 --activate #配置eth0网卡（--activate开机自启）

network --bootproto=static --device=eth1 --ip=172.16.1.63 --netmask=255.255.255.0 --activate #配置eth1网卡

network --hostname=Cobbler #设置主机名

#network --bootproto=dhcp --device=eth1 --onboot=yes --noipv6 --hostname=CentOS7            #可以使用dhcp方式设置网络

timezone --utc Asia/Shanghai #设置时区

authconfig --enableshadow --passalgo=sha512 #设置密码格式

rootpw --iscrypted $6$X20eRtuZhkHznTb4$dK0BJByOSAWSDD8jccLVFz0CscijS9ldMWwpoCw/ZEjYw2BTQYGWlgKsn945fFTjRC658UXjuocwJbAjVI5D6/ #密文密码

clearpart --all --initlabel      #清空分区

part /boot --fstype xfs --size 1024 #/boot分区

part swap --size 1024 #swap分区

part / --fstype xfs --size 1 --grow #根分区

firstboot --disable #负责协助配置redhat一些重要的信息

selinux --disabled #关闭selinux

firewall --disabled #关闭防火墙

logging --level=info #设置日志级别

reboot #安装完成重启



%packages             #包组段表示方法：@表示包组

@^minimal

@compat-libraries

@debugging

@development

tree #软件包

nmap

sysstat

lrzsz

dos2unix

telnet

wget

vim

bash-completion

%end

%post   #脚本段，可以放脚本或命令

systemctl disable postfix.service #关闭邮件服务开机自启动

%end
```

2. 无注释
```
install

#url --url="http://172.16.1.201/CentOS7/"
url --url=$tree

text

lang en_US.UTF-8

keyboard u

#
rootpw --iscrypted $default_password_crypted

zerombr

bootloader --location=mbr --driveorder=sda 

$SNIPPET('network_config')
#network --bootproto=static --device=eth0 --gateway=10.0.0.254 --ip=10.0.0.63 --nameserver=223.5.5.5 --netmask=255.255.255.0 --activate


#network --bootproto=static --device=eth1 --ip=172.16.1.63 --netmask=255.255.255.0 --activate

#network --hostname=Cobbler

#network --bootproto=dhcp --device=eth1 --onboot=yes --noipv6 --hostname=CentOS7

timezone --utc Asia/Shanghai

#authconfig --enableshadow --passalgo=sha512 
auth --useshadow --enablemd5


clearpart --all --initlabel

part /boot --fstype xfs --size 1024 --ondisk sda

part swap --size 1024 --ondisk sda

part / --fstype xfs --size 1 --grow --ondisk sda

firstboot --disable

selinux --disabled

firewall --disabled

logging --level=info

reboot

skipx


%pre
$SNIPPET('log_ks_pre')
$SNIPPET('kickstart_start')
$SNIPPET('pre_instlal_network_config')

$SNIPPET('pre_anamon')
%end

%packages

@^minimal

@ base
@ core

@compat-libraries

@debugging

@development

tree

nmap
sysstat
iptraf
ntp
lrzsz
ncurses-devel
openssl-devel
zlib-devel
OpenIPMI-tools
mysql
screen

dos2unix

telnet

wget

vim

bash-completion
%end

%post
systemctl disable postfix.service
%end


```
