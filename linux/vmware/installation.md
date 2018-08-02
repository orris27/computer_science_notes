## 1. Ubuntu上安装VMware Workstation
1. 版本检查
```
uname -r
# 4.13.0-46-generic
uname -m
# x86_64
```
2. 下载软件
> https://www.vmware.com/products/workstation-pro/workstation-pro-evaluation.html

3. 安装依赖包
```
sudo apt-get install murrine-themes
sudo apt-get install gtk2-engines-murrine
sudo apt-get install libgtkmm-2.4-1c2a
sudo apt-get install libgtkmm-2.4-dev
sudo apt-get install libcanberra-gtk-module:i386
sudo apt-get install gnome-tweak-tool
sudo apt-get install gksu
```
4. 执行安装软件
```
chmod +x VMware-Workstation-Full-14.1.2-8497320.x86_64.bundle
gksudo bash ./VMware-Workstation-Full-14.1.2-8497320.x86_64.bundle
```

5. 安装界面
    1. 根据提示输入选择Yes/No
    2. 名字那里写`orris`就好
    3. 存放目录我改成了`/var/lib/vmware/Shared-VMs/`
    4. 注册码激活一下
    5. 安装完成

## 2. 创建虚拟机
1. 点击菜单栏的File>New Virtual Machine
2. 选择Custom,点击Next
3. `Workstation 14.x`这里选择默认的就好
4. 选择I will install the opreating system later
+ 前面两项都不推荐
5. 选择的操作系统是Linux.然后Version那里选择`CentOS 64-bit`和`Other Linux 2.6.x kernel 64-bit`都可以,老师推荐选择内核版本
+ 老师这里安装的是CentOS6或5,所以内核版本低
+ 我选择了`Other Linux 3.x kernel 64-bit`(因为我的阿里云虚拟机上内核版本是`3.10.0-514.26.2.el7.x86_64`)
6. Name和Location自己选择
+ Location不建议放在系统盘
+ Location所在盘最好10G以上
+ 我选择了Name:`/home/orris/vmware/orris-centos`,而location为`/home/orris/vmware/orris-centos`
7. Processors这里因为我们只是学习,所以1个CPU且1核就可以了
8. 创建虚拟机时内存至少大于512MB,最好是1G以上
+ 刚开始安装系统时可以考虑2G,这样安装更快
+ 老师推荐是自己内存大小的一半

9. 网络系统
    1. 桥接模式(我们推荐)
    + 虚拟机和笔记本一样,直接上网,和宿主机没有关系
    + 适合生产环境,办公环境
    + 局域网254台机器可能ip冲突
    2. NAT => 因为老师讲课的时候还没有分配IP地址,所以老师选择了NAT
    + 适合家庭环境
    3. Use Host-only Network
    + 不能上网=>不推荐
    + 只能和宿主机打交道


10. IO控制器类型
+ 默认就行了

11. 磁盘:选择创建一个虚拟的磁盘

12. 磁盘类型: 选择SCSI
+ SAS和SATA都基于SCSI

13. 磁盘大小:选择10G
+ Allocate all disk space now:直接分出10G,对系统当然不好=>不要勾选
+ Split Virtual disk into multiple files=>选择这个就好(也是默认的)

14. Disk File:磁盘名称,选择默认不改

15. 选择Finish就完成了


## 3. 安装CentOS
1. 右击左侧栏中我们创建的虚拟机,选择Setting,在ISO处选择选择`CentOS-5.5-x86_64_bin-DVD.iso`
+ CD版比DVD版大很多,我们一般就选择DVD版
+ 选择x86_64,不要选i386
+ 我选择了`CentOS-7-x86_64-Minimal-1804.iso`
+ 其他保持默认,如选择了`Connect at power on`(这个默认勾选就行)
2. 点`Power on this virtual Machine`/`Start on this guest...`开始安装
+ 如果没有弹出安装界面=>按F2(需要重启虚拟机)可以调出虚拟机的BIOS,在BOOT下面将CD-ROM Drive移动到最上面
+ 老师设置成默认的,就是说没有将CD-ROM Drive放到最上面

3. 点击Install or upgrade an existing system
+ Install System with basic video driver几乎不用
+ Rescure installed system是救援系统=>Linux出现问题才使用
+ CentOS7没有这个,直接就安装了

4. Disc Found:要不要检查
+ 忽略检查光盘(这个很费时间,而且基本不做)

5. Installation Summary
    1. Language during the installation process:选择英文把
    + 是安装过程中选择什么语言,~~系统的语言~~
    
    2. 键盘选择默认的(英文)
    
    3. Language Support:选择英文(操作系统安装的语言版本)
    
    
    4. Network and Hostname
        1. Hostname:主机名,比如说选择template(这个主机作为模板)
        + ~~localhost~~,选择localhost肯定很危险
        2. 如果打开Network的话,后面我们就不用自己配置了
    
    5. Installation Destination
    + 选择I will configure partitioning
        1. 创建分区:选择Free,点击create.选择Standard Partition,点击Create

        2. 增加分区:/boot=>sda1
        + Mount Point: /boot
        + File System Type: ext4(CentOS7拿xfs做默认文件系统)
        + Size(MB): 100-200M(老师选了200M)
        + Additional Size Options:Fixed size(表示就是给200M)
        + Force to be a primary partition:让它成为主分区(不是必须的)

        3. 增加分区:swap
        + Mount Point: 无
        + File System Type: swap
        + Size(MB): 1G(内存空间的1.5M,因为我们的内存空间可能为512M)
        + Additional Size Options:Fixed size
        + Force to be a primary partition:让它成为主分区(不是必须的)

        4. 增加分区:/
        + Mount Point: /
        + File System Type: ext4
        + Additional Size Options:Fill to maximum allowable size(Size里面随便填没关系)
        + Force to be a primary partition:让它成为主分区(不是必须的)

        5. 选择Next

        6. 格式化警告=>选择要

        7. 把配置写入磁盘=>选择要(不会丢笔记本的数据,而是虚拟磁盘的数据)
        
        8. Storage Device Warning:要不要把虚拟的10G给删除掉:选择Yes
        + 因为我们要装系统,所以肯定要删除原来的数据(初始化)
    
    6. 时区选择亚洲上海
    + 时区很重要
    + UTC不必选择这个=>老师最后取消勾选`System clock uses UTC`
    
    7. 确认"SOFTWARE SELECTION"是否为"Minimal Install"，如果不是，则点击进去选择"Minimal Install"

    
6. Root Password和Confirm(再输入一遍):这里设置root的密码
+ 不用Create User,这是创建普通用户的意思
+ 系统此时已经正在安装所需要的包了

7. 安装完所需要的包后,点击Reboot
    
8. 进入CentOS后,输入`yum install net-tools -y`后,点击`ifconfig`,查看网络状态

    
7. 安装什么类型的设备=>选择Basic Storage Devices(基础的存储设备)就行了
+ ~~Specialized Storage Devices~~


8. 关闭CentOS
+ 右击左侧栏的虚拟机,选择Power>Power Off,直接确认

9. 将虚拟机的网络连接方式改成桥接模式
    1. 右击左侧栏的虚拟机,选择Setting
    2. Network Adapter下改成Bridged
    + Replicate不用勾选
10. 将虚拟机的驱动方式改成物理光驱,而非原来的CD-ROM
    1. 右击左侧栏的虚拟机,选择Setting
    2. DCD/DVD下改成Use physical drive
    + Replicate不用勾选

11. 启动虚拟机
    1. 出现提示`Cannot connect the virtual device ide1:0 because no corresponding device is available on the host.Do you want to try to connect this virtual device every time you power on the virtual machine?`,不用管他,直接yes

12. 关闭终端的警告音
```
vi /etc/inputrc
################
set bell-style none
################
reboot
```


13. 字体太小
+ 直接切换成ssh客户端


14. 安装setup(不需要)
+  按tab建切换到下面
```
yum install ntsysv -y
yum install iptables -y
yum install system-config-securitylevel-tui -y
setup
nmtui
```

15. 设置桥接模式
+ 其他字段保持不变就行了,我们只要修改/增加这些字段
+ GATEWAY根据我们的局域网决定,比如说在自己电脑上敲`route -n`,`0.0.0.0`栏的第一个就是默认网关了
+ 可以删除UUID,MAC等
```
vi /etc/sysconfig/network-scripts/ifcfg-ens33
##############################
DEVICE=ens33 # 自己的网卡
BOOTPROTO=static
IPADDR=192.168.1.12 # 必须和自己电脑在一个局域网且不被人占用
GATEWAY=192.168.1.1 # 跟自己电脑的默认网关一样
ONBOOT=yes
DNS1=8.8.8.8 # 大家都用8.8.8.8
DNS2=114.114.114.114
##############################
systemctl restart network
```
我个人的最终结果
```
cat /etc/sysconfig/network-scripts/ifcfg-ens33
################################
TYPE=Ethernet
PROXY_METHOD=none
BROWSER_ONLY=no
DEVICE=ens33
BOOTPROTO=static
IPADDR=192.168.1.2
GATEWAY=192.168.1.1
ONBOOT=yes
DNS1=8.8.8.8
DNS2=114.114.114.114
DEFROUTE=yes
IPV4_FAILURE_FATAL=no
IPV6INIT=yes
IPV6_AUTOCONF=yes
IPV6_DEFROUTE=yes
IPV6_FAILURE_FATAL=no
IPV6_ADDR_GEN_MODE=stable-privacy
NAME=ens33
################################
```

### 3-1. 其他

1. Linux需要引导,即为`Install boot loader on /dev/sda`(默认就行)
+ 选择默认的磁盘就好,如果有其他磁盘做引导的话,就选其他的

2. 安装哪些软件包
+ 选择Minimal(工作中)/Basic Server都行
+ 老师选择了最下面的Customize now(上面的包都不会安装),自定义
    1. `Customize now`的选择(总共6个)
        1. Application什么都不选
        2. Base System:
        + Base
        + Compatiblility libraries
        + Debugging Tools
        + Dial-up Networking Support
        + Hardware monitoring utilities
        + Performance tools
        3. Development
        + Development tools
    2. 如果安装时忘了选,就用`yum groupinstall`来安装
    + 如果忘记包名,就用`yum grouplist`


3. 重启
4. 关掉光盘(现在可能比较智能,自动就会关掉)
5. 检查网卡 ifconfig
    1. 输入`setup`
    2. 选择Network Configuration
    3. 选择Device Configuration
    4. eth0就是默认的第一块网卡,选择eth0,回车
    5. 默认情况下DHCP,要给DNS配置,老师是`8.8.8.8`.
    + 因为我们是NAT,所以选择DHCP就行
    6. `/etc/init.d/network restart`
    7. `/etc/sysconfig/`
    + ONBOOT=yes
    + UUID删除
    + MAC删除
`Ctrl+Alt`退出全屏
