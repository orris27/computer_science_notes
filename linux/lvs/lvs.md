## 1. 安装
ipvs的安装除了普通的编译安装外,更重要的是添加到系统内核中
1. 检测Linux内核是否已经有ipvs
2. 当前环境
3. 软连接
    + 将内核的目录软连接到`/usr/src/linux`
    + `/usr/src/kernels`下如果有多个目录就根据自己的内核参数决定,即选择`uname -r`
4. 安装依赖包
5. 常规的编译安装
6. 检测Linux内核是否已经有ipvs
7. 添加ipvs到内核中(下面两种方式任选1个)
    1. 方法1: `sudo /sbin/ipvsadm`
    2. 方法2: `modprobe ip_vs`
8. 检测Linux内核是否已经有ipvs
```
########################################################################
# 直接复制粘贴能成功!
########################################################################
lsmod | grep ip_vs # 如果什么都没有,就说明没有
cat /etc/redhat-release
uname -rm

sudo ln -s /usr/src/kernels/`uname -r`/ /usr/src/linux
sudo yum install kernel-devel -y
sudo yum install libnl* popt* -y
sudo yum install -y wget
sudo yum groupinstall "Development Tools" -y

mkdir ~/tools
cd ~/tools
wget http://www.linuxvirtualserver.org/software/kernel-2.6/ipvsadm-1.26.tar.gz
tar -zxf ipvsadm-1.26.tar.gz
cd ipvsadm-1.26
sudo make && sudo make install
lsmod | grep ip_vs
sudo /sbin/ipvsadm
modprobe ip_vs
lsmod | grep ip_vs
#----------------------------------------------------------
ip_vs                 141432  0 
nf_conntrack          133053  1 ip_vs
libcrc32c              12644  3 xfs,ip_vs,nf_conntrack
#----------------------------------------------------------
```
## 2. 配置
1. 配置VIP
2. 删除原来的LVS配置
3. 创建一个虚拟服务(ip+port组成)
4. 给虚拟服务添加真实服务器
5. 给真实服务器在环回接口上绑定虚拟服务的VIP
6. 对真实服务器抑制ARP响应
    > 修改内核参数就可以了
7. 永久生效(写脚本)
    > 上面写的内容都是临时生效

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
ipvsadm --add-server -t 10.0.0.10:80 --real-server 10.0.0.9 --gatewaying

ipvsadm --list -n

#####################################
# Web01 & Web02
#####################################
ifconfig lo:0 10.0.0.10/32 up
# 添加主机路由(仅仅route -n时多了一行,可选)
route add -host 10.0.0.10 dev eth0

# 1表示只回答目标IP地址是来访网络接口本地地址的ARP查询请求
echo "1" > /proc/sys/net/ipv4/conf/lo/arp_ignore # 原来是0
echo "2" > /proc/sys/net/ipv4/conf/lo/arp_announce
echo "1" > /proc/sys/net/ipv4/conf/all/arp_ignore
echo "2" > /proc/sys/net/ipv4/conf/all/arp_announce

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 可以写个脚本使永久生效
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++

curl 10.0.0.10:80
```



## 3. ipvsadm
ipvs的管理工具
### 3-1. options
1. `--add-service,-A`:add virtual service with options.LVS里服务是ip+port的形式
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

### 3-2. 实例
#### 3-2-1. 添加
1. virtual service
```
ipvsadm --add-service --tcp-service 10.0.0.10:80 --scheduler rr -p 20 
```
2. real server
```
ipvsadm --add-server --tcp-service 10.0.0.10:80 --real-server 10.0.0.8 --gatewaying
```
#### 3-2-2. 删除
1. real server
    > 指明服务和节点
```
ipvsadm --delete-server -t 172.19.28.82:80 -r 172.19.28.83:80
```
2. virtual server 
    > 指明服务
```
ipvsadm --delete-service -t 172.19.28.82:80
```
3. 所有服务
```
ipvsadm --clear
```
#### 3-2-3. 查看LVS情况
```
ipvsadm --list -n
```

## 4. arping
清空局域网内所有ARP表
```
arping -c 1 -I eth0 -s $VIP 10.0.0.254 > /dev/null 2>&1
```




