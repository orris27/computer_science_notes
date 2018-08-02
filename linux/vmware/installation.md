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
2. 点`Power on this virtual Machine`开始安装
+ 如果没有弹出安装界面=>按F2(需要重启虚拟机)可以调出虚拟机的BIOS,在BOOT下面将CD-ROM Drive移动到最上面
+ 老师设置成默认的,就是说没有将CD-ROM Drive放到最上面
3. 点击Install or upgrade an existing system
+ Install System with basic video driver几乎不用
+ Rescure installed system是救援系统=>Linux出现问题才使用
