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



## 2. 提示
1. 硬件CPU支持虚拟化技术,可以调整bios支持,特老机器只能安装32位
2. 创建虚拟机时内存至少大于512MB,最好是1G以上
3. 安装之后启动虚拟机至少512M内存,否则可能报错
安装时内存大点,建议自己内存的一半
