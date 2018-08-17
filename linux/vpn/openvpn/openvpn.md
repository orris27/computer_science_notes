    
## 1. 安装
1. 环境需求(老师的)
    1. 笔记本IP:10.0.0.0/24(办公室DHCP)
    2. OpenVPN server(双网卡)
        1. eth0:10.0.0.28(外网),网关,dns
        2. eth1:172.16.1.28(内网),不用网关(1个服务器不要配2个网关)
        3. 可以连接互联网
        4. 可以ping通172.16.1.17
    3. IDC机房内部局域网服务器:172.16.1.0/24
    3. APP server:
        1. eth0:172.16.1.17(内网)
        2. 网关:默认没有网关
    4. 实现需求:远端客户端能通过vpn客户端拨号到VPN server,然后可以直接访问vpnserver所在局域网内的多个servers

1. 环境需求(我的)
    1. 笔记本IP:192.168.1.0/24(办公室DHCP),192.168.1.36
    2. OpenVPN server(双网卡).桥接模式?(我设置了2个网卡都是桥接模式,不知道正确与否)
        1. eth0:192.168.1.100(外网),网关,dns
        2. eth1:172.16.1.28(内网),不用网关(1个服务器不要配2个网关)
        3. 可以连接互联网
        4. 可以ping通172.16.1.17
    3. IDC机房内部局域网服务器:172.16.1.0/24
    3. APP server:
        1. eth0:172.16.1.17(内网)
        2. 网关:默认没有网关
        3. 桥接模式?反正实验里我设置成了桥接模式...
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
openvpn --genkey --secret keys/ta.key
ll keys/ta.key 
#--------------------------------------------------------------------------------
# [root@vpn-server 2.0]# ll keys/ta.key 
# -rw-------. 1 root root 636 Aug 17 22:33 keys/ta.key
#--------------------------------------------------------------------------------
```
