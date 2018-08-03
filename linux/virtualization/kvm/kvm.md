## 1. 安装
1. 安装依赖包
```
yum install qemu-kvm qemu-kvm-tools virt-manager libvirt virt-install -y
```

2. 允许虚拟化
    1. 设置虚拟机内存大小为4GB
    2. 检查是否支持虚拟化
    + CPU的标志是否有vmx和svm
    + 虚拟机是否是VMware Workstation(VirtualBox不支持内嵌虚拟化)
    + 如果什么都没有显示的话,就说明不支持虚拟化
    ```
    egrep '(vmx|svm)' /proc/cpuinfo # 只要出现一个就说明支持虚拟化
    ```
    3. 如果不支持虚拟化,对于VMware Workstation来说,点击Setting>Process>勾选`Disable acceleration for binary translation`和`Virtualize Intel VT-x/EPT or AMD-V/RVI`
    + 如果不能勾选的话,是因为内dev存太小的缘故,设置内存为4GB
    + 因为我这里的选项不一样而且三个,所以我三个都勾选,包括`Intel VT-x/EPT or AMD-V/RVI`,`CPU`和`IOMMU`
    
3. 创建一个硬盘
+ `-f`:指定文件格式
+ 指定路径
+ 指定大小
```
qemu-img create -f raw /opt/CentOS-7.5-x86_64.raw 10G
```
4. 检查内核模块是否加载进来了
```
lsmod | grep kvm 
################################################## 出现下面内容就是ok了
kvm_intel             174841  0 
kvm                   578518  1 kvm_intel
##################################################
```

5. 查看libvirtd状态
```
systemctl enable libvirtd.service
systemctl start libvirtd.service
systemctl status libvirtd.service
```

6. 用dd命令挂载一块iso镜像
+ 虚拟机Setting>CD/DVD>勾选Use ISO image,选择CentOS7的iso镜像后,`/dev/cdrom`就是这个镜像位置了
```
dd if=/dev/cdrom of=/opt/CentOS-7.5.iso
################################################ 结果
1855488+0 records in
1855488+0 records out
950009856 bytes (950 MB) copied, 70.9196 s, 13.4 MB/s
################################################
```



7. 下载VNC客户端
    1. 去官网下载deb包
    > https://www.realvnc.com/en/connect/download/viewer/linux/
    2. 直接双击deb包,点击Install就行了
    + 如果没有显示进度条,不要着急,它其实已经在安装了


8. 安装内嵌的虚拟机
+ 区名称
+ 内存大小
+ CPU信息(默认1个1核CPU)
+ 指定安装用的光驱
+ 执行磁盘路径
+ 指定虚拟化的类型
```
virt-install --help
virt-install --name CentOS-7.5-x86_64 --virt-type kvm --ram 1024 --cdrom=/opt/CentOS-7.5.iso --disk path=/opt/CentOS-7.5-x86_64.raw --network network=default --graphics vnc,listen=0.0.0.0 --noautoconsole
#################################################################### 结果
Starting install...
Domain installation still in progress. You can reconnect to 
the console to complete the installation process.
####################################################################

```

9. 使用VNC客户端安装内嵌的CentOS
    1. 点击VNC客户端,输入`192.168.1.2:5900`,然后按tab键,追加net.ifnames=0 biosdevname=0
    + ip地址是我们的虚拟机的地址
    + 端口是从5900开始
    
    2. 安装CentOS-7.5
    + 老师只进行了手动分区
        + swap 2G
        + / 7991M左右
        + /boot 1G
  
    + 时区老师没有改变
    + 没有主动打开网络,即OFF状态
    + 老师设置了root密码
    + 我手动分区了,并且时区改成中国,网络没有打开,设置了root密码


10. 查看
```
ps -axu| grep kvm
cd /etc/libvirt/qemu生成了xml,不要修改,要修改使用`virsh edit`
```

11. 虚拟机管理工具
+ virt-manager:图形界面

```

libvirt
```


