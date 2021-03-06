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
    + 如果不能勾选的话,主要原因是虚拟机没有关机,主要power off就行了.也可能是因为内dev存太小的缘故,设置内存为4GB
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
systemctl enable libinfovirtd.service
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
```start
ps -axu| grep kvm
cd /etc/libvirt/qemu # 生成了xml,不要修改,要修改使用`virsh edit`
cat CentOS-7.5-x86_64.xml # 自动生成的,我们不要管
```

11. 安装好CentOS后,点击Reboot(密码设置好,安装好后下面就有Reboot),VNC客户端会退出


12. 利用virsh启动虚拟机
```
virsh list --all # 获得名字
########################################################## 结果
 Id    Name                           State
----------------------------------------------------
 -     CentOS-7.5-x86_64              shut off
##########################################################


# 通过这里的名字启动
virsh start CentOS-7.5-x86_64
########################################################## 结果
Domain CentOS-7.5-x86_64 started
##########################################################
```

13. 使用VNC客户端连接,直接输入`192.168.1.2:5900`或者直接点击VNC客户端里的黑框就行
+ 需要等待一会儿时间(因为在启动),之后就进来了



14. 输入`ip ad li`查看ip情况,发现没有IP地址
+ `ifconfig`默认没有安装
+ 没有IP地址是因为没有启动网卡(设置里面没有说开机自启动网卡)

15. 启动网卡
    1. 设置网卡开机自启动
    ```
    vi /etc/sysconfig/network-scripts/ifcfg-eth0
    ###############
    ONBOOT=yes
    ###############
    ```
    2. 重启网卡
    ```
    systemctl restart network
    ```
    3. 查看网络状态,发现ip地址是192.168.122.102
    + 这个ip地址是因为内嵌虚拟机使用了虚拟机的dnsmasq服务
    ```
    ip ad li
    ```
    4. 查看主虚拟机的dnsmasq服务的配置文件,里面的dhcp-range就是分配的范围
    ```
    ps aux | grep dns # --conf-file就是配置文件位置
    cat /var/lib/libvirt/dnsmasq/default.conf
    ########################################################
    dhcp-range=192.168.122.2,192.168.122.254
    ########################################################
    ```
    5. 安装ifconfig
    ```
    # ping baidu.com # 发现能ping通,说明可以上网了
    yum install -y net-tools
    ```

16. 配置CPU并热添加
    1. 设置CPU参数
    + 通过XML指定
    + 设置成1个CPU,最大值为4个CPU=>CentOS可以热添加CPU
    + 也可以安装的时候`virt-install`来指定
    ```
    virsh list # 得到名字,复制到下面
    virsh edit CentOS-7.5-x86_64
    ########################### 将static改成auto模式
    # 原来的:<vcpu placement='static'>1</vcpu>
    <vcpu placement='auto' current='1'>4</vcpu>
    ###########################
    ```
    2. 关闭虚拟机
    + 改变XML都要重启才行
    ```
    virsh shutdown CentOS-7.5-x86_64
    virsh list
    virsh list --all
    ```
    3. 重新启动虚拟机
    ```
    virsh start CentOS-7.5-x86_64
    ```
    
    4. 热添加CPU
    + CentOS7才能热添加CPU
    + 不支持热删除
    + 热添加的个数也不能超过最大值
    ```
    ##################################
    # 内嵌虚拟机
    ##################################
    cat /proc/cpuinfo
    
    ##################################
    # 主虚拟机
    ##################################    
    virsh setvcpus CentOS-7.5-x86_64 2 --live
    
    
    ##################################
    # 内嵌虚拟机
    ##################################
    # cat /sys/devices/system/cpu/cpu{0,1}/online # 有些版本可能要给这个文件echo 1才能生效
    # 也可能不需要echo 1
    cat /proc/cpuinfo
    ```


17. 配置内存并热添加/删除
    1. 设置内存参数
    + 通过XML指定
    + 设置最大的内存空间
    + 也可以安装的时候`virt-install`来指定
    ```
    virsh list # 得到名字,复制到下面
    virsh edit CentOS-7.5-x86_64
    ########################### 
    # 原来的:<memory unit='KiB'>1048576</memory>
    <memory unit='KiB'>4048576</memory>
    ###########################
    ```
    2. 关闭虚拟机
    + 改变XML都要重启才行
    + 关闭虚拟机需要一定时间,如果`virsh list`里面不显示了,就说明关闭成功
    ```
    virsh shutdown CentOS-7.5-x86_64
    virsh list
    virsh list --all
    ```
    3. 重新启动虚拟机
    ```
    virsh start CentOS-7.5-x86_64
    ```
    
    4. 热添加/删除内存
    ```
    ##################################
    # 内嵌虚拟机
    ##################################
    free -m
    
    ##################################
    # 主虚拟机
    ##################################    
    virsh qemu-monitor-command --help # <domain>是这里的名字
    virsh qemu-monitor-command CentOS-7.5-x86_64 --hmp --cmd info balloon # 查看信息
    virsh qemu-monitor-command CentOS-7.5-x86_64 --hmp --cmd balloon 2000
    
    ##################################
    # 内嵌虚拟机
    ##################################
    free -m
    ```


18. 设置虚拟机为网桥模式
    1. 查看网桥情况
    + 如果没有`brctl`的话,就要安装`bridge-utils`这个包
    ```
    ##################################
    # 主虚拟机
    ##################################  
    brctl show
    ```
    2. 添加一个网桥
    ```
    ##################################
    # 主虚拟机
    ##################################  
    brctl addbr br0
    brctl show
    ```
    3. 将eth0加入到网桥
    + 断网问题:实际工作中可以写脚本来执行,虚拟机的环境的话,使用VMware Workstation来连接
    + br0没有IP,而eth0有IP,关联br0
    ```
    ##################################
    # 主虚拟机
    ##################################  
    brctl addif br0 eth0 # 执行后会断网,ssh会连接不上
    ```
    4. 启动网桥
    + 如果提示说`Unreachable`的话,注意是否执行`ifconfig br0 192.168.1.2/24 up`
    ```
    ##################################
    # 主虚拟机
    ################################## 
    ifconfig
    brctl show
    ip addr del dev eth0 192.168.1.2/24
    ifconfig br0 192.168.1.2/24 up
    
    ip ro li # 发现没有路由=>添加默认网关(也有情况自己不用添加默认网关)
    route add defaultt gw 192.168.1.1
    # systemctl stop iptables
    # systemctl stop firewalld
    ```
    5. 验证
    + 使用ssh连接验证就好


19. 删除虚拟机
    1. 彻底删除虚拟机
    ```
    virsh undefine
    ```
    2. 如果删除后意外/想要恢复,注意备份`/etc/libvirt/qemu`
    ```
    virsh --help | grep dump # dumpxml可以管理
    ```
    
    
20. 目前为止主虚拟机和内嵌虚拟机的端口状态
```
#####################################################################################################
# 主虚拟机
#####################################################################################################
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name    
tcp        0      0 127.0.0.1:25151         0.0.0.0:*               LISTEN      772/python2         
tcp        0      0 0.0.0.0:873             0.0.0.0:*               LISTEN      519/rsync           
tcp        0      0 0.0.0.0:5900            0.0.0.0:*               LISTEN      3770/qemu-kvm       
tcp        0      0 0.0.0.0:111             0.0.0.0:*               LISTEN      524/rpcbind         
tcp        0      0 192.168.122.1:53        0.0.0.0:*               LISTEN      1391/dnsmasq        
tcp        0      0 0.0.0.0:22              0.0.0.0:*               LISTEN      770/sshd            
tcp        0      0 127.0.0.1:25            0.0.0.0:*               LISTEN      1010/master         
tcp6       0      0 :::873                  :::*                    LISTEN      519/rsync           
tcp6       0      0 :::111                  :::*                    LISTEN      524/rpcbind         
tcp6       0      0 :::80                   :::*                    LISTEN      778/httpd           
tcp6       0      0 :::22                   :::*                    LISTEN      770/sshd            
tcp6       0      0 ::1:25                  :::*                    LISTEN      1010/master         
tcp6       0      0 :::443                  :::*                    LISTEN      778/httpd           
udp        0      0 127.0.0.1:323           0.0.0.0:*                           546/chronyd         
udp        0      0 0.0.0.0:693             0.0.0.0:*                           524/rpcbind         
udp        0      0 192.168.122.1:53        0.0.0.0:*                           1391/dnsmasq        
udp        0      0 0.0.0.0:67              0.0.0.0:*                           4286/dhcpd          
udp        0      0 0.0.0.0:67              0.0.0.0:*                           1391/dnsmasq        
udp        0      0 0.0.0.0:69              0.0.0.0:*                           775/xinetd          
udp        0      0 0.0.0.0:111             0.0.0.0:*                           524/rpcbind         
udp6       0      0 ::1:323                 :::*                                546/chronyd         
udp6       0      0 :::693                  :::*                                524/rpcbind         
udp6       0      0 :::111                  :::*                                524/rpcbind      

#####################################################################################################
# 内嵌虚拟机
#####################################################################################################
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name    
tcp        0      0 0.0.0.0:22              0.0.0.0:*               LISTEN      822/sshd            
tcp        0      0 127.0.0.1:25            0.0.0.0:*               LISTEN      1297/master         
tcp6       0      0 :::22                   :::*                    LISTEN      822/sshd            
tcp6       0      0 ::1:25                  :::*                    LISTEN      1297/master         
udp        0      0 0.0.0.0:68              0.0.0.0:*                           644/dhclient   


```
