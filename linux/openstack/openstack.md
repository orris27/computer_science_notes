## 1. 安装
1. 环境准备
    1. CentOS-7.1 系统2台,每台2G内存
    + linux-node1.oldboyedu.com 192.168.56.11 网卡NAT eth0 控制节点
    + linux-node2.oldboyedu.com 192.168.56.12 网卡NAT eth0 计算节点
        1. NAT实现外网+与物理机不同网段的实现
        + vment8       != 菜单栏的网卡   = 虚拟机的网卡  != 虚拟机的IP地址
        + 192.168.56.1 != 192.168.56.2 =192.168.56.2 != 192.168.56.11        
        > https://github.com/orris27/orris/blob/master/linux/vmware/installation.md
        2. 改变网卡名称
        > https://github.com/orris27/orris/blob/master/network/network.md
        
    
    2. 域名解析
    + OpenStack主机名很重要
    ```
    cat >> /etc/hosts <<EOF
    192.168.56.11 linux-node1 linux-node1.oldboyedu.com
    192.168.56.12 linux-node2 linux-node2.oldboyedu.com
    EOF
    ```
    3. 硬盘50G
    
    4. Tightvnc view VNC客户端
    
    5. 安装包:老师发给大家
    
    6. 理解SOA,RestAPI,消息队列,对象存储
    
    7. 使用L版OpenStack
