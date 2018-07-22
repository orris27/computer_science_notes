## 1. 安装
ipvs的安装除了普通的编译安装外,更重要的是添加到系统内核中
### 1-1. 检测Linux内核是否已经有ipvs
```
lsmod | grep ip_vs # 如果什么都没有,就说明没有
```
### 1-2. 当前环境
```
cat /etc/redhat-release
uname -rm
```
### 1-3. 软连接
将内核的目录软连接到`/usr/src/linux`
+ `/usr/src/kernels`下如果有多个目录就根据自己的内核参数决定,即选择`uname -r`
```
sudo ln -s /usr/src/kernels/3.10.0-862.6.3.el7.x86_64.debug/ /usr/src/linux
```
### 1-4. 安装依赖包
```
sudo yum install kernel-devel -y
sudo yum install libnl* popt* -y

```
### 1-5. 常规的编译安装
```
wget http://www.linuxvirtualserver.org/software/kernel-2.6/ipvsadm-1.26.tar.gz
tar -zxf ipvsadm-1.26.tar.gz
cd ipvsadm-1.26
sudo make 
sudo make install
```
### 1-6. 检测Linux内核是否已经有ipvs
```
lsmod | grep ip_vs
```
### 1-7. 添加ipvs到内核中
#### 方法1.
```
sudo /sbin/ipvsadm
```
#### 方法2.
```
modprobe ip_vs
```
### 1-8. 检测Linux内核是否已经有ipvs
```
lsmod | grep ip_vs
```
### 2. 配置
+ 可能VIP应该选一个新的,然后配合在调度器的`eth0:0`上
+ 可能调度器和真实服务器都要`route add -host 172.19.28.xx dev eth0`(真实服务器应该是lo)
#### 2-1. 虚拟节点
##### 2-1-0. ipvsadm
> https://github.com/orris27/orris/blob/master/linux/shell/command.md
##### 2-1-1. 清空当前列表
```
ipvsadm -C
```
##### 2-1-2. 设置tcp,tcpfin,udp的超时时间(可选)
```
ipvsadm --set 30 5 60
```
##### 2-1-3. 添加virtual server
设置`172.19.28.82:80`为虚拟节点
+ 最好写明端口
+ 这里写的`-p 22`不是必需的
```
ipvsadm -A -t 172.19.28.82:80 -s rr -p 20
```
##### 2-1-4. 添加real server
在`172.19.28.82:80`的虚拟节点上添加real server `172.19.28.8x:80`,工作模式为DR,权重为1
```
ipvsadm -a -t 172.19.28.82:80 -r 172.19.28.83:80 -g -w 1
ipvsadm -a -t 172.19.28.82:80 -r 172.19.28.84:80 -g -w 1
```
##### 2-1-5. 检查是否添加成功
```
ipvsadm -L -n
```
#### 2-2. Real Server
##### 2-2-1. 绑定VIP
在Real Server上的环回接口(LO)设备上绑定VIP地址(其广播地址是其本身,子网掩码是255.255.255.255,采取可变长掩码方式把网段划分成只含一个主机地址的目的xx,避免ip地址冲突).
+ 只能在阿里云的管理终端上配置
```
ifconfig lo:0 172.19.28.82/32 up
```
##### 2-2-2. 抑制ARP
当客户的数据包来到服务器集群的局域网时,需要知道VIP是谁.如果不抑制ARP的话,那么所有节点都会响应.通过抑制ARP,我们只允许调度器接收数据包
```
echo "1" > /proc/sys/net/ipv4/conf/lo/arp_ignore # 原来是0
echo "2" > /proc/sys/net/ipv4/conf/lo/arp_announce
echo "1" > /proc/sys/net/ipv4/conf/all/arp_ignore
echo "2" > /proc/sys/net/ipv4/conf/all/arp_announce
```


### 3. 常见问题
#### 3-1. 
```
In file included from libipvs.h:13:0,
                 from libipvs.c:23:
ip_vs.h:15:29: fatal error: netlink/netlink.h: No such file or directory
 #include <netlink/netlink.h>
 compilation terminated.
make[1]: *** [libipvs.o] Error 1
make[1]: Leaving directory `/home/orris/tools/ipvsadm-1.26/libipvs'
make: *** [libs] Error 2
```
##### 解决
```
sudo yum install libnl* popt* -y
```
#### 3-2. 在阿里云的远程连接里设置好LO的IP后,virutal server就ping不通real server了
可能阿里云的服务器不支持把.只能在虚拟机做的样子
