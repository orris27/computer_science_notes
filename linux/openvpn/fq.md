## 1. 安装
> [参考资料](http://www.huizhanzhang.com/2017/05/bandwagon-one-key-shadowsocks.html)

1. 选择OpenVZ架构的服务器,地点选洛杉矶

2. 可以填写优惠码

3. 选择AliPay,并勾选同意合同

4. 点击Pay Now

5. 这时邮箱会收到类似下面的信息
```
Hi,

I have allocated the following IP address to your VPS:
23.106.131.162

For security reasons I no longer send root passwords via email.
Please set your root password in the KiwiVM panel.

I have automatically changed SSH port of your VPS to prevent automated brute-force attacks.
New SSH port: 27252

Yours truly,
KiwiVM IP Allocation Robot
```

6. 我们在My Services那里进入到刚才服务器的KiwiVM的控制面板

7. 先关机,然后在Install New OS那里安装CentOS6

8. 然后会告诉我们root的密码和ssh端口27252
    + 原本的密码我保存在我自己qq邮箱里了,但是我现在改成了自己常用的密码
    
9. 可以选择界面上的自己安装(可以参考文档),不过我这边还是选择手动安装玩玩

10. 下载一键安装脚本
```
mkdir ~/tools
cd ~/tools
wget --no-check-certificate -O shadowsocks-all.sh https://raw.githubusercontent.com/teddysun/shadowsocks_install/master/shadowsocks-all.sh
chmod +x shadowsocks-all.sh
./shadowsocks-all.sh 2>&1 | tee shadowsocks-all.log
#------------------------------------------------------------------------
Which Shadowsocks server you'd select:
1) Shadowsocks-Python
2) ShadowsocksR
3) Shadowsocks-Go
4) Shadowsocks-libev
Please enter a number (Default Shadowsocks-Python): # 我选择了直接回车

You choose = Shadowsocks-Python

Please enter password for Shadowsocks-Python
(Default password: teddysun.com):123456 # 我选择密码是123456

password = 123456

Please enter a port for Shadowsocks-Python [1-65535]
(Default port: 19607): # 我选择直接回车,端口是19607

port = 19607

Please select stream cipher for Shadowsocks-Python:
1) aes-256-gcm
2) aes-192-gcm
3) aes-128-gcm
4) aes-256-ctr
5) aes-192-ctr
6) aes-128-ctr
7) aes-256-cfb
8) aes-192-cfb
9) aes-128-cfb
10) camellia-128-cfb
11) camellia-192-cfb
12) camellia-256-cfb
13) xchacha20-ietf-poly1305
14) chacha20-ietf-poly1305
15) chacha20-ietf
16) chacha20
17) salsa20
18) rc4-md5
Which cipher you'd select(Default: aes-256-gcm):7

cipher = aes-256-cfb

# 加密方式选择AES-256-CFB
#------------------------------------------------------------------------

```
