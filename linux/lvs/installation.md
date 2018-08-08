## 1. 安装
ipvs的安装除了普通的编译安装外,更重要的是添加到系统内核中
1. 检测Linux内核是否已经有ipvs
```
lsmod | grep ip_vs # 如果什么都没有,就说明没有
```
2. 当前环境
```
cat /etc/redhat-release
uname -rm
```
3. 软连接
将内核的目录软连接到`/usr/src/linux`
+ `/usr/src/kernels`下如果有多个目录就根据自己的内核参数决定,即选择`uname -r`
```
sudo ln -s /usr/src/kernels/3.10.0-862.6.3.el7.x86_64.debug/ /usr/src/linux
```
4. 安装依赖包
```
sudo yum install kernel-devel -y
sudo yum install libnl* popt* -y

```
5. 常规的编译安装
```
#yum install -y wget
#yum groupinstall "Development Tools" -y

wget http://www.linuxvirtualserver.org/software/kernel-2.6/ipvsadm-1.26.tar.gz
tar -zxf ipvsadm-1.26.tar.gz
cd ipvsadm-1.26
sudo make && sudo make install
```
6. 检测Linux内核是否已经有ipvs
```
lsmod | grep ip_vs
```
7. 添加ipvs到内核中(下面两种方式任选1个)
    1. 方法1
    ```
    sudo /sbin/ipvsadm
    ```
    2. 方法2
    ```
    modprobe ip_vs
    ```
8. 检测Linux内核是否已经有ipvs
```
lsmod | grep ip_vs
#----------------------------------------------------------
ip_vs                 141432  0 
nf_conntrack          133053  1 ip_vs
libcrc32c              12644  3 xfs,ip_vs,nf_conntrack
#----------------------------------------------------------
```
## 2. 配置
用户访问的地址(VIP)是
1. 配置VIP
2. 删除原来的LVS配置
```
#####################################
# LVS01
#####################################
ifconfig eth0:0 10.0.0.10/24 up
# 添加主机路由(仅仅route -n时多了一行,可选)
route add -host 10.0.0.10 dev eth0

ipvsadm --help # ipvsadm是用来管理LVS的

ipvsadm --set 30 5 60 # 设置tcp,tcpfin,udp的超时时间(可选)
ipvsadm -C

# 添加一个虚拟IP:PORT为10.0.0.10:10的服务,调度算法使用rr,超时时间是20秒
ipvsadm --add-service -t 10.0.0.10:80 --scheduler rr -p 20 

# 给10.0.0.10:80这个虚拟ip:port添加一个节点,其真实服务器IP是10.0.0.8,采用DR模式
ipvsadm --add-server -t 10.0.0.10:80 --real-server 10.0.0.8 --gatewaying
```



## 3. ipvsadm
ipvs的管理工具
### 36-1. option
1. `--add-service,-A`:add virtual service with options
2. `--delete-service,-D`:delete virtual service
3. `--clear,-C`:clear the whole table
4. `--add-server,-a`:add real server with options
5. `--delete-server,-d`:delete real server
6. `--list,-L|-l`:list the table
7. `--persistent,-p[timeout]`:persistent service.超时时间
8. `--weight,-w`:capacity of real server
9. `--scheduler,-s scheduler`:one of rr|wrr|lc|wlc|lblc|lblcr|dh|sh|sed|nq, the default is wlc
10. `--set tcp tcpfin udp`:set connection timeout values
11. `--tcp-service,-t service-address`:service-address is host[:port]
12. `--real-server,-r server-address`:server-address is host (and port)
13. `--gatewaying,-g`:gatewaying (direct routing) (default)

### 36-2. 实战
#### 36-2-1. 删除real server
```
ipvsadm -d -t 172.19.28.82:80 -r 172.19.28.83:80
```
#### 36-2-2. 删除virtual server
```
ipvsadm -D -t 172.19.28.82:80
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
