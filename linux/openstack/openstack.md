## 1. 安装
1. 环境准备
    1. CentOS-7.1 系统2台,每台2G内存
        1. linux-node1.oldboyedu.com 192.168.56.11 网卡NAT eth0
        2. linux-node2.oldboyedu.com 192.168.56.12 网卡NAT eth0
    
    2. 域名解析
    + OpenStack主机名很重要
        1. 点击菜单栏Edit>Virtual Network Editor>选择NAT模式,调整子网IP,取消勾选使用本地DHCP服务将IP地址分给虚拟机
    ```
    192.168.56.11 linux-node1 linux-node1.oldboyedu.com
    192.168.56.12 linux-node2 linux-node2.oldboyedu.com
    ```
    3. 硬盘50G
    
    4. Tightvnc view VNC客户端
    
    5. 安装包:老师发给大家
    
    6. 理解SOA,RestAPI,消息队列,对象存储
    
    7. 使用L版OpenStack
