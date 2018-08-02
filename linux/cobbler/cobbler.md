## 1. 安装
1. yum安装
```
yum install cobbler cobbler-web dhcp tftp-server pykickstart httpd -y
```


2. 重启Apache
```
systemctl restart httpd
```

3. 启动Cobbler
```
systemctl start cobblerd
```
4. 检测Cobbler
```
cobbler check
```
5. 通过`cobbler check`可以得到提示,我们根据提示解决问题
    1. 修改`/etc/cobbler/settings`
    ```
    manage_dhcp: 1
    next_server: 192.168.1.2
    server: 192.168.1.2
    ```
    2. 设置密码
        1. 生成密码
        ```
        openssl passwd -1 -salt 'cobbler' 'cobbler' # 前面是盐,后面是我们的密码明文
        # $1$cobbler$M6SE55xZodWc9.vAKLJs6.
        ```
        2. 将输出的密码放到配置文件里
        ```
        # vim /etc/cobbler/settings
        ######################################################################
        default_password_crypted: "$1$cobbler$M6SE55xZodWc9.vAKLJs6."
        ######################################################################
        ```
    3. 去网上下载资源
    ```
    cobbler get-loaders
    ```
    4. (只有老师那里有这个要求)将`vim /etc/xinetd.d/rsync`的disable改成`no`,并且重启xinetd
    
    5. 修改`/etc/selinux/config`文件,将`SELINUX=enforcing`改为`SELINUX=disabled`,重启电脑
    + 注意要执行`systemctl start httpd;systemctl enable httpd;systemctl start cobblerd`
    
    6. 启动rsyncd,并设置开机自启动
    ```
    systemctl start rsyncd
    systemctl enable rsyncd
    ```
    
    7. 以下提示可以无视
    + 当然可以用`yum install -y debmirror yum-utils fence-agents`,不过老师最后没有装
    ```
    1 : debmirror package is not installed, it will be required to manage debian deployments and repositories
    2 : fencing tools were not found, and are required to use the (optional) power management features. install cman or fence-agents to use them
    ```
    
    
```
######################## 发现有下面的提示,之后根据提示解决问题
The following are potential configuration items that you may want to fix:

1 : The 'server' field in /etc/cobbler/settings must be set to something other than localhost, or kickstarting features will not work.  This should be a resolvable hostname or IP for the boot server as reachable by all machines that will use it.
2 : For PXE to be functional, the 'next_server' field in /etc/cobbler/settings must be set to something other than 127.0.0.1, and should match the IP of the boot server on the PXE network.
3 : SELinux is enabled. Please review the following wiki page for details on ensuring cobbler works correctly in your SELinux environment:
    https://github.com/cobbler/cobbler/wiki/Selinux
4 : Some network boot-loaders are missing from /var/lib/cobbler/loaders, you may run 'cobbler get-loaders' to download them, or, if you only want to handle x86/x86_64 netbooting, you may ensure that you have installed a *recent* version of the syslinux package installed and can ignore this message entirely.  Files in this directory, should you want to support all architectures, should include pxelinux.0, menu.c32, elilo.efi, and yaboot. The 'cobbler get-loaders' command is the easiest way to resolve these requirements.
5 : enable and start rsyncd.service with systemctl
6 : debmirror package is not installed, it will be required to manage debian deployments and repositories
7 : The default password used by the sample templates for newly installed machines (default_password_crypted in /etc/cobbler/settings) is still set to 'cobbler' and should be changed, try: "openssl passwd -1 -salt 'random-phrase-here' 'your-password-here'" to generate new one
8 : fencing tools were not found, and are required to use the (optional) power management features. install cman or fence-agents to use them

Restart cobblerd and then run 'cobbler sync' to apply changes.
########################
```


## 2. 配置
1. 设置dhcp,修改`/etc/cobbler/dhcp.template`,并执行
+ 下面的内容都要修改成对应的值
+ 每次修改`dhcp.template`后都要执行`cobbler sync`
+ 执行完`cobbler sync`后就会对应修改`/etc/dhcp/dhcpd.conf `
```
vim /etc/cobbler/dhcp.template
################################
subnet 192.168.1.0 netmask 255.255.255.0 { # 子网+子网掩码
     option routers             192.168.1.1; # 我们所在的默认网关
     option domain-name-servers 192.168.1.210,192.168.1.211; # DNS服务器
     option subnet-mask         255.255.255.0; # 子网掩码
     range dynamic-bootp        192.168.1.100 192.168.1.200; # dhcp分配域名的范围
#...
################################
cobbler sync
```

2. 导入镜像文件
+ 最终位置是在`/var/www/cobbler/ks_mirror/`
```
# mount /dev/cdrom /mnt
cobbler import --path=/mnt/ --name=CentOS-7.5-x86_64 --arch=x86_64
```

3. 将Kickstart文件放到指定的目录下`/var/lib/cobbler/kickstarts/`
+ 配置文件放在下面连接中的无注释版里
> https://github.com/orris27/orris/blob/master/linux/cobbler/kickstart.md
```
mv CentOS-7.5-x86_64.cfg /var/lib/cobbler/kickstarts/
```
4. 查看Cobbler为我们创建的仓库和配置
```
cobbler list
```

5. 修改cobbler list,并执行
```
cobbler profile report # 查看状态
cobbler profile edit --help # 查看帮助
cobbler profile edit --name=CentOS-7.5-x86_64 --kickstart=/var/lib/cobbler/kickstarts/CentOS-7.5-x86_64.cfg
# cobbler list
cobbler profile edit --name=CentOS-7.5-x86_64 --kopts='net.ifnames=0 biosdevname=0'
# cobbler profile report
cobbler sync
```

6. 更改Cobbler安装提示,并sync执行
```
vim /etc/cobbler/pxe/pxedefault.template
##########################################################################
# MENU TITLE Cobbler | http://cobbler.github.io/ 可以改成自己想要的
MENU TITLE Cobbler By orris | http://www.orris.com/
##########################################################################
cobbler sync
```
7. 检测其他主机是否能连接上我们
```
curl 192.168.1.2 #最好在局域网的其他主机上也测试
#systemctl stop firewalld
#systemctl stop iptables
```

8. 最后我的Cobbler服务器上的端口情况
```
netstat -lntup
#######################################################################################################
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name    
tcp        0      0 127.0.0.1:25151         0.0.0.0:*               LISTEN      1913/python2        
tcp        0      0 0.0.0.0:873             0.0.0.0:*               LISTEN      502/rsync           
tcp        0      0 0.0.0.0:22              0.0.0.0:*               LISTEN      862/sshd            
tcp        0      0 127.0.0.1:25            0.0.0.0:*               LISTEN      1067/master         
tcp6       0      0 :::873                  :::*                    LISTEN      502/rsync           
tcp6       0      0 :::80                   :::*                    LISTEN      865/httpd           
tcp6       0      0 :::22                   :::*                    LISTEN      862/sshd            
tcp6       0      0 ::1:25                  :::*                    LISTEN      1067/master         
tcp6       0      0 :::443                  :::*                    LISTEN      865/httpd           
udp        0      0 127.0.0.1:323           0.0.0.0:*                           514/chronyd         
udp        0      0 0.0.0.0:67              0.0.0.0:*                           1635/dhcpd          
udp        0      0 0.0.0.0:69              0.0.0.0:*                           867/xinetd          
udp6       0      0 ::1:323                 :::*                                514/chronyd         
#######################################################################################################
```

9. 创建一个新的虚拟机,并且通过BIOS启动
    1. 创建虚拟机
    + 创建了个名字为`orris-test`
    + 内存设置为2G
    + 网络类型选择Bridged(非常重要)
    + 过程如下
    > https://github.com/orris27/orris/blob/master/linux/vmware/installation.md

    2. 修改虚拟机的网络类型为桥接模式
    3. 点击菜单栏VM>Power>Power On to Firmware
    4. 进入Boot里面,将`Network boot from Intel E1000`移动到最上面.保存并退出
    5. 重新启动虚拟机,选择`(local)`下面的`CentOS-7.5-xxx`,回车即开始安装

10. 安装完成后,由于还是网卡启动,所以进入`(local)`和`CentOS-7.5-xxx`的选项,选择local

11. 用户名为root,密码为cobbler,这样就能登录成功了!!!

12. 问题如`you have specified that the group base should be installed`和`cobbler PXE-E53:No boot filename received`等见下面的网站
> https://github.com/orris27/orris/blob/master/linux/installation_error.md


## 3. 定制
1. 获取mac地址,如`00:0C:29:96:5F:3D`
+ Setting>Network Adapter>Advanced>MAC Address

2. 为这个主机定制主机名,ip地址等
+ profile:指定在用网卡启动时,使用`(local)`还是`CentOS-7.5-x86_64`
+ 移除的话使用`cobbler system remove --name=orris`
```
cobbler system add --name=orris --mac=00:0C:29:96:5F:3D --profile=CentOS-7.5-x86_64 --ip-address=192.168.1.100 --subnet=255.255.255.0 --gateway=192.168.1.1 --interface=eth0 --static=1 --hostname=orris.example.com --name-servers="8.8.8.8 114.114.114.114"
```
3. 检查状态
```
cobbler system list # 能看到orris
```
4. 启动测试用虚拟机,如果不询问我直接安装,就说明安装成功


## 4. cobbler_web 
1. 在浏览器里输入`https://192.168.1.2/cobbler_web`
+ 一定是`https`


2. 用户名和密码都是cobbler
+ 修改用户名和密码在`/etc/cobbler/users.conf`+`/etc/cobbler/users.digest`


