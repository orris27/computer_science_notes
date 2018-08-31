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
23.106.xxx.xxx

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

10. 安装SS
    1. 下载一键安装脚本
    2. 执行一键安装脚本
    3. 输入密码,端口,加密模式等信息,就会自动安装了
    4. 安装完后就会弹出信息
```
mkdir ~/tools
cd ~/tools
wget --no-check-certificate -O shadowsocks-all.sh https://raw.githubusercontent.com/teddysun/shadowsocks_install/master/shadowsocks-all.sh
# 也可以选择我github上的auto-install-ss.sh,2个是一样的
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
Which cipher you'd select(Default: aes-256-gcm):7 # 加密方式选择AES-256-CFB

cipher = aes-256-cfb


Press any key to start...or Press Ctrl+C to cancel # 回车就可以安装了

#####################################################################################
# 成功的话输出如下
#####################################################################################

INFO: loading config from /etc/shadowsocks-python/config.json
2018-08-10 22:09:32 INFO     loading libcrypto from libcrypto.so.10
Starting Shadowsocks success

Congratulations, Shadowsocks-Python server install completed!
Your Server IP        :  23.106.xxx.xxx
Your Server Port      :  19607 
Your Password         :  123456 
Your Encryption Method:  aes-256-cfb 

Your QR Code: (For Shadowsocks Windows, OSX, Android and iOS clients)
 ss://YWVzLTI1Ni1jZmI6MTIzNDU2QDIzLjEwNi4xMzEuMTYyOjE5NjA3 
Your QR Code has been saved as a PNG file path:
 /root/tools/shadowsocks_python_qr.png 

Welcome to visit: https://teddysun.com/486.html
Enjoy it!


#------------------------------------------------------------------------

```
11. Ubuntu使用SS
    1. 下载shadowxxx
    2. 创建连接
        1. Server Address填自己VPS的公有IP地址:`23.106.x.xx`
        2. Server Port填:`19607`
        3. Password填SS的密码:`123456`
        4. Local Address默认选择`127.0.0.1`就行,如果要提供给其他人用的话,这里可能要变更下
        5. Local Port填在自己的电脑上开的端口,如`1080`
        6. Local Server Type填:`SOCKS5`
        7. 加密方式选择`AES-256-CFB`
    3. 点击OK,并Connect,如果Status下面显示Connected就说明成功了.
        + 如果失败请检查上面是否都填正确,比如加密方式有没有选择错误等
        
    4. Chrome浏览器上安装SwitchyOmega插件
    5. 添加Profile,填写信息
        1. Protocol选择SOCKS5
        2. Server选择`127.0.0.1`
        3. Port选择`1080`
        4. Bypass List
        ```md
        127.0.0.1
        [::1]
        localhost
        *.bilibili.com # 规则就是这样.很简单
        ```
        5. Apply Changes
    6. 然后在浏览器右上角点击它,选择对应的Profile就可以连接了
        
    
12. 手机端的wingy
1. 选择类型:Shadowsocks
2. 填写相关信息就好
