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

## 2. 安装Linux
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
    2. NAT
    + 适合家庭环境
    3. Use Host-only Network
    + 不能上网=>不推荐
    + 只能和宿主机打交道


### 2-1. 提示
1. 硬件CPU支持虚拟化技术,可以调整bios支持,特老机器只能安装32位
2. 创建虚拟机时内存至少大于512MB,最好是1G以上
3. 安装之后启动虚拟机至少512M内存,否则可能报错
安装时内存大点,建议自己内存的一半
