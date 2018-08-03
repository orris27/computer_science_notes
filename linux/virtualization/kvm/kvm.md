## 1. 安装
1. 安装依赖包
```
yum install qemu-kvm qemu-kvm-tools virt-manager libvirt virt-install -y
```

2. 允许虚拟化
    1. 设置虚拟机内存大小为4GB
    2. 检查是否支持虚拟化
    + CPU的标志是否有vmx和svm
    + 如果什么都没有显示的话,就说明不支持虚拟化
    ```
    egrep '(vmx|svm)' /proc/cpuinfo # 只要出现一个就说明支持虚拟化
    ```
    3. 如果不支持虚拟化,对于VMware Workstation来说,点击Setting>Process>勾选`Disable acceleration for binary translation`和`Virtualize Intel VT-x/EPT or AMD-V/RVI`
    + 如果不能勾选的话,是因为内存太小的缘故,设置内存为4GB
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
+ 
```
dd if=/dev/cdrom of=/opt/CentOS-7.5.iso
```




