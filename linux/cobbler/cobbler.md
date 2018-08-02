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
     option domain-name-servers 8.8.8.8; # DNS服务器
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
7. 创建一个新的虚拟机,并且通过BIOS启动
    1. 创建虚拟机
    + 创建了个名字为`orris-test`
    + 内存设置为2G
    + 网络类型选择NAT
    + 过程如下
    > https://github.com/orris27/orris/blob/master/linux/vmware/installation.md
    2. 点击菜单栏VM>Power>Power On to Firmware


