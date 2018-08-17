## 1. 安装
### 1-1. OpenVPN服务端安装与配置
1. 环境需求(我的)
    1. 笔记本IP:192.168.1.0/24(办公室DHCP),192.168.1.36
    2. OpenVPN server(双网卡).桥接模式(我设置了2个网卡都是桥接模式,不知道正确与否=>最终实现了,所以是可以的)
        1. eth0:192.168.1.100(外网),网关,dns
        2. eth1:172.16.1.28(内网),不用网关(1个服务器不要配2个网关)
        3. 可以连接互联网
        4. 可以ping通172.16.1.17
    3. IDC机房内部局域网服务器:172.16.1.0/24
    3. APP server:
        1. eth0:172.16.1.17(内网)
        2. 网关:默认没有网关
        3. 桥接模式?反正实验里我设置成了桥接模式...=>最终实现了,所以是可以的
    4. 实现需求:远端客户端能通过vpn客户端拨号到VPN server,然后可以直接访问vpnserver所在局域网内的多个servers
2. 确认系统版本
```
[root@vpn-server ~]# cat /etc/redhat-release 
CentOS Linux release 7.5.1804 (Core) 
[root@vpn-server ~]# uname -r
3.10.0-862.el7.x86_64
[root@vpn-server ~]# uname -m
x86_64
```
3. 安装软件包
```
yum install -y wget ntpdate openssl-devel
yum groupinstall -y "Development Tools"
```
4. 时间同步
```
/usr/sbin/ntpdate pool.ntp.org

echo '#time sec' >> /var/spool/cron/root
echo '*/5 * * * * /usr/sbin/ntpdate pool.ntp.org >/dev/null 2>&1' >> /var/spool/cron/root
crontab -l
```

5. 安装软件
```
########################################################################
# VPN-Server
########################################################################
mkdir ~/tools
cd ~/tools

# 最多使用的其实是lzo的2.02和2.0.9版本,是最稳定的,这里还是使用了较新版
wget http://www.oberhumer.com/opensource/lzo/download/lzo-2.06.tar.gz
tar zxf lzo-2.06.tar.gz
cd lzo-2.06
./configure
make && make install


cd ../
wget http://build.openvpn.net/downloads/releases/openvpn-2.2.2.tar.gz
tar zxf openvpn-2.2.2.tar.gz
cd openvpn-2.2.2
./configure --with-lzo-headers=/usr/local/include --with-lzo-lib=/usr/local/lib
make && make install

which openvpn # 如果有就表示安装好了
```

6. 创建CA证书
```
########################################################################
# VPN-Server
########################################################################
cd ~/tools/openvpn-2.2.2/easy-rsa/2.0/
cp vars vars.bak.20180817
ls vars*
#------------------------------------------------------------
# vars  vars.bak.20180817
#------------------------------------------------------------
vi vars
################################################################
export KEY_COUNTRY="CN"
export KEY_PROVINCE="BJ"
export KEY_CITY="Beijing"
export KEY_ORG="oldboy"
export KEY_EMAIL="49000448@qq.com"
export KEY_EMAIL=49000448@qq.com
export KEY_CN=CN
export KEY_NAME=oldboy
export KEY_OU=oldboy
export PKCS11_MODULE_PATH=changeme
export PKCS11_PIN=1234
################################################################

source vars
./clean-all  # 删除之前的key
./build-ca # 只有Common Name那里需要自己写,其他都回车默认
#--------------------------------------------------------------------------
# Generating a 1024 bit RSA private key
# ........................++++++
# ..............++++++
# writing new private key to 'ca.key'
# -----
# You are about to be asked to enter information that will be incorporated
# into your certificate request.
# What you are about to enter is what is called a Distinguished Name or a DN.
# There are quite a few fields but you can leave some blank
# For some fields there will be a default value,
# If you enter '.', the field will be left blank.
# -----
# Country Name (2 letter code) [CN]:
# State or Province Name (full name) [BJ]:
# Locality Name (eg, city) [Beijing]:
# Organization Name (eg, company) [oldboy]:
# Organizational Unit Name (eg, section) [oldboy]:
# Common Name (eg, your name or your server's hostname) [CN]:oldboy
# Name [oldboy]:
# Email Address [49000448@qq.com]:
#--------------------------------------------------------------------------

ll keys/
#--------------------------------------------------------------------------
# total 12
# -rw-r--r--. 1 root root 1310 Aug 17 22:05 ca.crt
# -rw-------. 1 root root  916 Aug 17 22:05 ca.key
# -rw-r--r--. 1 root root    0 Aug 17 22:04 index.txt
# -rw-r--r--. 1 root root    3 Aug 17 22:04 serial
#--------------------------------------------------------------------------

```

7. 生成服务端的秘钥
```
########################################################################
# VPN-Server
########################################################################
./build-key-server server # server为名字
# password和compnay name里写了,其他都默认或者yes.这里的password是在申请CA证书时会用到的
#--------------------------------------------------------------------------------
# Generating a 1024 bit RSA private key
# ...++++++
# ....++++++
# writing new private key to 'server.key'
# -----
# You are about to be asked to enter information that will be incorporated
# into your certificate request.
# What you are about to enter is what is called a Distinguished Name or a DN.
# There are quite a few fields but you can leave some blank
# For some fields there will be a default value,
# If you enter '.', the field will be left blank.
# -----
# Country Name (2 letter code) [CN]:
# State or Province Name (full name) [BJ]:
# Locality Name (eg, city) [Beijing]:
# Organization Name (eg, company) [oldboy]:
# Organizational Unit Name (eg, section) [oldboy]:
# Common Name (eg, your name or your server's hostname) [server]:
# Name [oldboy]:
# Email Address [49000448@qq.com]:
# 
# Please enter the following 'extra' attributes
# to be sent with your certificate request
# A challenge password []:123456
# An optional company name []:oldboy
# Using configuration from /root/tools/openvpn-2.2.2/easy-rsa/2.0/openssl-1.0.0.cnf
# Check that the request matches the signature
# Signature ok
# The Subject's Distinguished Name is as follows
# countryName           :PRINTABLE:'CN'
# stateOrProvinceName   :PRINTABLE:'BJ'
# localityName          :PRINTABLE:'Beijing'
# organizationName      :PRINTABLE:'oldboy'
# organizationalUnitName:PRINTABLE:'oldboy'
# commonName            :PRINTABLE:'server'
# name                  :PRINTABLE:'oldboy'
# emailAddress          :IA5STRING:'49000448@qq.com'
# Certificate is to be certified until Aug 14 14:16:24 2028 GMT (3650 days)
# Sign the certificate? [y/n]:y
# 
# 
# 1 out of 1 certificate requests certified, commit? [y/n]y
# Write out database with 1 new entries
# Data Base Updated # 出现更新了就说明成功
#--------------------------------------------------------------------------------


ll keys/ | grep server # 生成的秘钥
#--------------------------------------------------------------------------------
# -rw-r--r--. 1 root root 3995 Aug 17 22:16 server.crt # crt为证书
# -rw-r--r--. 1 root root  769 Aug 17 22:16 server.csr
# -rw-------. 1 root root  916 Aug 17 22:16 server.key # key为秘钥文件
#--------------------------------------------------------------------------------

```
8. 生成客户端的证书和秘钥
+ 服务端和客户端各有自己的免秘钥,但共用CA证书
+ 使用`build-key`来创建用户的话,在拨号的时候不会要密码,可以使用`build-key-pass`替代
```
########################################################################
# VPN-Server(无密码版本)
########################################################################
./build-key test # 在工作中后面的test一般为人的名字
#--------------------------------------------------------------------------------
# Generating a 1024 bit RSA private key
# ....................++++++
# ......++++++
# writing new private key to 'test.key'
# -----
# You are about to be asked to enter information that will be incorporated
# into your certificate request.
# What you are about to enter is what is called a Distinguished Name or a DN.
# There are quite a few fields but you can leave some blank
# For some fields there will be a default value,
# If you enter '.', the field will be left blank.
# -----
# Country Name (2 letter code) [CN]:
# State or Province Name (full name) [BJ]:
# Locality Name (eg, city) [Beijing]:
# Organization Name (eg, company) [oldboy]:
# Organizational Unit Name (eg, section) [oldboy]:
# Common Name (eg, your name or your server's hostname) [test]:
# Name [oldboy]:
# Email Address [49000448@qq.com]:
# 
# Please enter the following 'extra' attributes
# to be sent with your certificate request
# A challenge password []:123456
# An optional company name []:oldboy
# Using configuration from /root/tools/openvpn-2.2.2/easy-rsa/2.0/openssl-1.0.0.cnf
# Check that the request matches the signature
# Signature ok
# The Subject's Distinguished Name is as follows
# countryName           :PRINTABLE:'CN'
# stateOrProvinceName   :PRINTABLE:'BJ'
# localityName          :PRINTABLE:'Beijing'
# organizationName      :PRINTABLE:'oldboy'
# organizationalUnitName:PRINTABLE:'oldboy'
# commonName            :PRINTABLE:'test'
# name                  :PRINTABLE:'oldboy'
# emailAddress          :IA5STRING:'49000448@qq.com'
# Certificate is to be certified until Aug 14 14:21:45 2028 GMT (3650 days)
# Sign the certificate? [y/n]:y
# 
# 
# 1 out of 1 certificate requests certified, commit? [y/n]y
# Write out database with 1 new entries
# Data Base Updated
#--------------------------------------------------------------------------------

ll keys/ | grep test
#--------------------------------------------------------------------------------
# -rw-r--r--. 1 root root 3871 Aug 17 22:21 test.crt
# -rw-r--r--. 1 root root  765 Aug 17 22:21 test.csr
# -rw-------. 1 root root  916 Aug 17 22:21 test.key
#--------------------------------------------------------------------------------





########################################################################
# VPN-Server(有密码版本)
########################################################################
./build-key-pass ett 

# 一开始就要求输入密码,这个密码就是拨号时候要输入的密码,我这里写的是123456
# A challenge password和An optional company name要填写,其他默认或yes
#--------------------------------------------------------------------------------
# Generating a 1024 bit RSA private key
# ..........++++++
# .........................++++++
# writing new private key to 'ett.key'
# Enter PEM pass phrase:
# Verifying - Enter PEM pass phrase:
# -----
# You are about to be asked to enter information that will be incorporated
# into your certificate request.
# What you are about to enter is what is called a Distinguished Name or a DN.
# There are quite a few fields but you can leave some blank
# For some fields there will be a default value,
# If you enter '.', the field will be left blank.
# -----
# Country Name (2 letter code) [CN]:
# State or Province Name (full name) [BJ]:
# Locality Name (eg, city) [Beijing]:
# Organization Name (eg, company) [oldboy]:
# Organizational Unit Name (eg, section) [oldboy]:
# Common Name (eg, your name or your server's hostname) [ett]:
# Name [oldboy]:
# Email Address [49000448@qq.com]:
# 
# Please enter the following 'extra' attributes
# to be sent with your certificate request
# A challenge password []:123456
# An optional company name []:oldboy
# Using configuration from /root/tools/openvpn-2.2.2/easy-rsa/2.0/openssl-1.0.0.cnf
# Check that the request matches the signature
# Signature ok
# The Subject's Distinguished Name is as follows
# countryName           :PRINTABLE:'CN'
# stateOrProvinceName   :PRINTABLE:'BJ'
# localityName          :PRINTABLE:'Beijing'
# organizationName      :PRINTABLE:'oldboy'
# organizationalUnitName:PRINTABLE:'oldboy'
# commonName            :PRINTABLE:'ett'
# name                  :PRINTABLE:'oldboy'
# emailAddress          :IA5STRING:'49000448@qq.com'
# Certificate is to be certified until Aug 14 14:25:59 2028 GMT (3650 days)
# Sign the certificate? [y/n]:y
# 
# 
# 1 out of 1 certificate requests certified, commit? [y/n]y
# Write out database with 1 new entries
# Data Base Updated
#--------------------------------------------------------------------------------


ll keys/| grep ett
#--------------------------------------------------------------------------------
# -rw-r--r--. 1 root root 3870 Aug 17 22:26 ett.crt
# -rw-r--r--. 1 root root  765 Aug 17 22:25 ett.csr
# -rw-------. 1 root root 1041 Aug 17 22:25 ett.key
#--------------------------------------------------------------------------------
```
9. 生成传输进行秘钥交换时用到的交换秘钥协议文件
```
########################################################################
# VPN-Server
########################################################################
./build-dh

ll keys/dh1024.pem # 秘钥协议文件
#--------------------------------------------------------------------------------
# -rw-r--r--. 1 root root 245 Aug 17 22:29 keys/dh1024.pem
#--------------------------------------------------------------------------------
ll keys/
#--------------------------------------------------------------------------------
# [root@vpn-server 2.0]# ll keys/
# total 84
# -rw-r--r--. 1 root root 3995 Aug 17 22:16 01.pem
# -rw-r--r--. 1 root root 3871 Aug 17 22:21 02.pem
# -rw-r--r--. 1 root root 3870 Aug 17 22:26 03.pem
# -rw-r--r--. 1 root root 1310 Aug 17 22:05 ca.crt # 服务端和所有客户端都需要
# -rw-------. 1 root root  916 Aug 17 22:05 ca.key # 服务端需要
# -rw-r--r--. 1 root root  245 Aug 17 22:29 dh1024.pem # 协议文件,只在server里由
# -rw-r--r--. 1 root root 3870 Aug 17 22:26 ett.crt # 客户端使用
# -rw-r--r--. 1 root root  765 Aug 17 22:25 ett.csr
# -rw-------. 1 root root 1041 Aug 17 22:25 ett.key # 客户端使用
# -rw-r--r--. 1 root root  355 Aug 17 22:26 index.txt
# -rw-r--r--. 1 root root   21 Aug 17 22:26 index.txt.attr
# -rw-r--r--. 1 root root   21 Aug 17 22:21 index.txt.attr.old
# -rw-r--r--. 1 root root  238 Aug 17 22:21 index.txt.old
# -rw-r--r--. 1 root root    3 Aug 17 22:26 serial
# -rw-r--r--. 1 root root    3 Aug 17 22:21 serial.old
# -rw-r--r--. 1 root root 3995 Aug 17 22:16 server.crt # 服务端的证书
# -rw-r--r--. 1 root root  769 Aug 17 22:16 server.csr # 服务端的key
# -rw-------. 1 root root  916 Aug 17 22:16 server.key
# -rw-r--r--. 1 root root 3871 Aug 17 22:21 test.crt # 客户端使用
# -rw-r--r--. 1 root root  765 Aug 17 22:21 test.csr
# -rw-------. 1 root root  916 Aug 17 22:21 test.key # 客户端使用
#--------------------------------------------------------------------------------
```

10. 生成一个"HAMC firewall"来防止恶意攻击,如DOS,UDP port flooding
```
########################################################################
# VPN-Server
########################################################################
openvpn --genkey --secret keys/ta.key
ll keys/ta.key 
#--------------------------------------------------------------------------------
# [root@vpn-server 2.0]# ll keys/ta.key 
# -rw-------. 1 root root 636 Aug 17 22:33 keys/ta.key
#--------------------------------------------------------------------------------
```

11. 将OpenVPN的配置文件和证书集中管理
```
########################################################################
# VPN-Server
########################################################################
mkdir -p /etc/openvpn # 专门存放openvpn证书以及配置文件的地方
cd ~/tools/openvpn-2.2.2/easy-rsa/2.0/
cp -ap keys /etc/openvpn/
cd ~/tools/openvpn-2.2.2/sample-config-files/
cp client.conf server.conf /etc/openvpn/

yum install -y tree

tree /etc/openvpn/
#--------------------------------------------------------------------------------
# [root@vpn-server sample-config-files]# tree /etc/openvpn/
# /etc/openvpn/
# ├── client.conf
# ├── keys
# │   ├── 01.pem
# │   ├── 02.pem
# │   ├── 03.pem
# │   ├── ca.crt
# │   ├── ca.key
# │   ├── dh1024.pem
# │   ├── ett.crt
# │   ├── ett.csr
# │   ├── ett.key
# │   ├── index.txt
# │   ├── index.txt.attr
# │   ├── index.txt.attr.old
# │   ├── index.txt.old
# │   ├── serial
# │   ├── serial.old
# │   ├── server.crt
# │   ├── server.csr
# │   ├── server.key
# │   ├── ta.key
# │   ├── test.crt
# │   ├── test.csr
# │   └── test.key
# └── server.conf
# 
# 1 directory, 24 files
#--------------------------------------------------------------------------------


cd /etc/openvpn/
cp server.conf server.conf.bak
grep -vE ';|#|^$' server.conf
#上面2个是没有被grep到的,而我来添加的(为了说明作用)
#--------------------------------------------------------------------------------
# port 1194
# proto udp
# dev tun
# ca ca.crt
# cert server.crt
# dh dh1024.pem
# server 10.8.0.0 255.255.255.0
# ifconfig-pool-persist ipp.txt
# keepalive 10 120
# comp-lzo
# persist-key
# persist-tun
# status openvpn-status.log
# verb 3
#--------------------------------------------------------------------------------

grep -vE ';|#|^$' server.conf >tmp.log
cat tmp.log > server.conf
vi server.conf
#############################################################################################
local 192.168.1.100
port 52115
proto tcp
dev tun
ca /etc/openvpn/keys/ca.crt
cert /etc/openvpn/keys/server.crt
key /etc/openvpn/keys/server.key
dh /etc/openvpn/keys/dh1024.pem
server 10.8.0.0 255.255.255.0
push "route 172.16.1.0 255.255.255.0"
ifconfig-pool-persist ipp.txt
keepalive 10 120
comp-lzo
persist-key
persist-tun
status openvpn-status.log
verb 3
client-to-client
duplicate-cn
log /var/log/openvpn.log
#############################################################################################
```

12. 调试服务端VPN服务启动环境
    1. 关闭防火墙,selinux
    2. 允许ip转发
```
########################################################################
# VPN-Server
########################################################################
vi /etc/sysconfig/iptables
#############################################################################################
-A INPUT -i lo -j ACCEPT # 这里是有的,我这里写在这里是为了说明下面一行是加到这个位置的下面的
-A INPUT -p tcp --dport 52115 -j ACCEPT
#############################################################################################
iptables -A INPUT -p tcp --dport 52115 -j ACCEPT
systemctl stop iptables
systemctl stop firewalld
setenforce 0
vi /etc/sysconfig/selinux 
#############################################################################################
SELINUX=disabled
#############################################################################################



vi /etc/sysctl.conf 
#############################################################################################
net.ipv4.ip_forward = 1
#############################################################################################
sysctl  -p
```
13. 启动VPN Sever
```
#/usr/local/sbin/openvpn --daemon --writepid /var/run/openvpn/server.pid --config server.conf --cd /etc/openvpn # OpenVPN自带的启动方法
/usr/local/sbin/openvpn --config /etc/openvpn/server.conf &
#-----------------------------------------------------------------------------------
# [1] 30071 # 如果只出现这个就说明ok了
#-----------------------------------------------------------------------------------

netstat -lntup | grep 52115

echo "#startup openvpn service" >>/etc/rc.local 
echo "/usr/local/sbin/openvpn --config /etc/openvpn/server.conf &" >>/etc/rc.local
tail -2 /etc/rc.local 
#-----------------------------------------------------------------------------------
# #startup openvpn service
# /usr/local/sbin/openvpn --config /etc/openvpn/server.conf &
#-----------------------------------------------------------------------------------

tail -100 /var/log/openvpn.log # 如果启动不了就会告诉我们为什么启动不了

ifconfig

#-----------------------------------------------------------------------------------
# tun0: flags=4305<UP,POINTOPOINT,RUNNING,NOARP,MULTICAST>  mtu 1500
#         inet 10.8.0.1  netmask 255.255.255.255  destination 10.8.0.2
#         inet6 fe80::196d:c160:23ed:3105  prefixlen 64  scopeid 0x20<link>
#         unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 100  (UNSPEC)
#         RX packets 0  bytes 0 (0.0 B)
#         RX errors 0  dropped 0  overruns 0  frame 0
#         TX packets 3  bytes 144 (144.0 B)
#         TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
#-----------------------------------------------------------------------------------
```

### 1-2. OpenVPN客户端安装与配置
#### 1-2-1. Windows下的OpenVPN GUI
```
cd /etc/openvpn/keys/
sz -y ca.crt test.crt test.key
cd /etc/openvpn/
sz -y client.conf
放到OpenVPN GUI的config/目录下面,创建1个test目录,放到里面(config/test/)
新建test.ovpn并配置下面内容.(注意test.crt和test.key要和配置文件在同级目录)
####################################################################################
client
dev tun
proto tcp
remote 192.168.1.100 52115
resolv-retry infinite
nobind
persist-key
persist-tun
ca ca.crt
cert test.crt
key test.key
ns-cert-type server
comp-lzo
verb 3
####################################################################################
点击下面的Connect,如果绿色的就表示连接陈宫
而且在服务器的日志里也会看到该客户端连接进来了

现在还ping不通172.16.1.17.能到达但并不能回来,所以要在内网服务器上配置路由
可以用该命令查看情况tcpdump -nnn -s 1000| grep -i icmp
##############################################################################
# 172.16.1.17
##############################################################################
route add default gw 172.16.1.28
route -n 
##############################################################################
# 笔记本电脑
##############################################################################
ping 172.16.1.17
```
### 1-2-2. CentOS7/Ubuntu下的OpenVPN客户端
1. 安装OpenVPN客户端
2. 在服务器上将ca证书,客户端的配置文件,用户的证书和秘钥发送到远程用户(CentOS7用户)的配置文件下
3. 启动客户端
4. 在VPN Server所在内网的主机(`172.16.1.28`)上配置出去的路由
5. ping内网主机,验证
```
##############################################################################
# 模拟远程用户的主机,CentOS7系统,192.168.1.200,虚拟机为桥接模式
##############################################################################
yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
yum install -y openvpn
#sudo apt-get install openvpn # 如果是Ubuntu的话只要安装这个就好了,其他和CentOS7的完全一样



##############################################################################
# VPN服务器 172.16.1.28(192.168.1.100)
##############################################################################
scp client.conf keys/ca.crt keys/test.key keys/test.crt root@192.168.1.200:/etc/openvpn/client


cd /etc/openvpn/client/
mv client.conf client.ovpn
vi client.ovpn
####################################################################################
client
dev tun
proto tcp
remote 192.168.1.100 52115 # 这里填写VPN服务器的IP和PORT
resolv-retry infinite
nobind
persist-key
persist-tun
ca ca.crt # 下面三个文件必须要和客户端的配置文件在同级目录,否则要写绝对路径
cert test.crt
key test.key
ns-cert-type server
comp-lzo
verb 3
####################################################################################

ping 192.168.1.100
ping 172.16.1.28
ping 172.16.1.17
openvpn --daemon --cd /etc/openvpn/client --config client.ovpn --log-append /var/log/openvpn.log
ifconfig # 会发现VPN服务器给我们分配了一个IP地址10.8.0.6
#-----------------------------------------------------------------------------------------------------------
# tun0: flags=4305<UP,POINTOPOINT,RUNNING,NOARP,MULTICAST>  mtu 1500
#         inet 10.8.0.6  netmask 255.255.255.255  destination 10.8.0.5
#         inet6 fe80::d116:162:aa47:d219  prefixlen 64  scopeid 0x20<link>
#         unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 100  (UNSPEC)
#         RX packets 3  bytes 252 (252.0 B)
#         RX errors 0  dropped 0  overruns 0  frame 0
#         TX packets 39  bytes 3168 (3.0 KiB)
#         TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
#-----------------------------------------------------------------------------------------------------------
route -n # 会发现VPN服务器给我们配置了172.16.1.0的路由
#-----------------------------------------------------------------------------------------------------------
# Kernel IP routing table
# Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
# 0.0.0.0         192.168.1.1     0.0.0.0         UG    0      0        0 eth0
# 10.8.0.0        10.8.0.5        255.255.255.0   UG    0      0        0 tun0
# 10.8.0.5        0.0.0.0         255.255.255.255 UH    0      0        0 tun0
# 172.16.1.0      10.8.0.5        255.255.255.0   UG    0      0        0 tun0
# 192.168.1.0     0.0.0.0         255.255.255.0   U     100    0        0 eth0
#-----------------------------------------------------------------------------------------------------------
ping 172.16.1.28 # ping通
ping 172.16.1.17 # ping不通,因为有去无回.(可以在内网服务器上用tcpdump看能否抓取到)









##############################################################################
# 内网主机172.16.1.17
##############################################################################
route add default gw 172.16.1.28
route -n


##############################################################################
# 模拟远程用户的主机,CentOS7系统,192.168.1.200,虚拟机为桥接模式
##############################################################################
ping 172.16.1.17 # ping通!
```

## 2. 详解
### 2-1. 命令
1. `vars`:用来创建环境变量,设置所需要的变量的脚本
2. `clean-all`:清除所有生成的ca证书以及秘钥文件的文件及目录
3. `build-ca`:生成ca证书(交互)
4. `build-dh`:生成Diffie-Hellman文件(交互)
5. `build-key-server`:生成服务端秘钥(交互)
6. `build-key`,`build-key-pass`:生成客户端秘钥(交互)
7. `pkitool`:直接使用vars的环境变量设置,直接生成证书(非交互)
### 2-2. 配置文件
主要是server和push这2个配置非常重要
```
local 124.43.12.115 # [修改成外网ip地址].哪一个本地地址要被OpenVPN进行监听.客户端要访问VPN服务器的地址
push "route 172.16.1.0 255.255.255.0" # [第二个ip地址使用的是我们内网服务器的网段]这是VPN Server所在的内网网段,如果有多个可以写多个push,注意:此命令实际作用是在VPN客户端本地生成VPN Server所在的内网网段的路由,确保能够和VPN Server所在的内网网段通信.路由条目类似:10.0.0.0 255.255.255.0 10.8.0.9 10.8.0.10 1.可以用route print在执行前后监视,然后用比较工具来比较就能知道变化在哪里了.push表示推到客户端
port 1194 # 监听的端口,默认为1194,为了安全可以修改
proto udp # 监听的协议,当并发多的时候,推荐tcp
dev tun # vpn server的模式采用路由的模式,可选tap或tun
ca ca.crt # ca证书,注意此文件和server.conf在同一个目录下,否则要用绝对路径调用
cert server.crt
dh dh1024.pem
server 10.8.0.0 255.255.255.0 # 这个是VPN server动态分配给VPN Client的地址池,一般不需要修改.不要和其他网段冲突
ifconfig-pool-persist ipp.txt
keepalive 10 120 # 每10秒ping一次,若是120秒未收到包,即认定客户端断线
comp-lzo # 开启压缩功能
persist-key # 当vpn超时后,当重新启动VPN后,保持上一次使用的私钥,而不重新读取私钥
persist-tun # 通过keepalived检测VPN超时后,当重新启动VPN后,保持tun或者tap设备自动连接状态
status openvpn-status.log # OpenVPN日志状态信息
log /var/log/openvpn.log # 日志文件
verb 3 # 指定日志文件的冗余
cient-to-client # 允许拨号的多个VPN Client互相通信
duplicat-cn # 允许多个客户端使用同一个账号连接
```
