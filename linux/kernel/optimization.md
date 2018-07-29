## Linux内核优化
具体的优化参数根据服务器的不同要求会有所不同,下面的代码只是一个参考,以后我会考虑弄个更加专门的内核参数
+ 文件描述符加大
+ ip_local_port_range的范围可以扩大
- 进程在随机选取端口监听的时候,只会在这个范围内寻找端口监听
### 1. 下面的代码追加到系统内核(`/etc/sysctl.conf`)
```
net.ipv4.tcp_fin_timeout = 2

net.ipv4.tcp_tw_recycle = 1

net.ipv4.tcp_tw_reuse = 1

net.ipv4.tcp_syncookies = 1

net.ipv4.tcp_keepalive_time = 600

net.ipv4.ip_local_port_range = 10000 65000

net.ipv4.tcp_max_syn_backlog = 16384

net.ipv4.tcp_max_tw_buckets = 36000

net.ipv4.route.gc_timeout = 100

net.ipv4.tcp_syn_retries = 1

net.ipv4.tcp_synack_retries = 1

net.core.somaxconn = 16384

net.core.netdev_max_backlog = 16384

net.ipv4.tcp_max_orphans = 16384

net.nf_conntrack_max = 25000000

net.ipv4.tcp_keepalive_probes = 3

net.ipv4.tcp_keepalive_intvl =15

net.ipv4.tcp_retries2 = 5

net.ipv4.tcp_wmem = 8192 131072 16777216

net.ipv4.tcp_rmem = 32768 131072 16777216

net.ipv4.tcp_mem = 786432 1048576 1572864

net.ipv4.ip_conntrack_max = 65536

net.ipv4.netfilter.ip_conntrack_max=65536

net.ipv4.netfilter.ip_conntrack_tcp_timeout_established=180

```
### 2. 使生效
```
sudo sysctl -p
```
### 3. 一些问题
#### 3-1.
```
sysctl: cannot stat /proc/sys/net/ipv4/ip_conntrack_max: No such file or directory
sysctl: cannot stat /proc/sys/net/ipv4/netfilter/ip_conntrack_max: No such file or directory
sysctl: cannot stat /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established: No such file or directory
```
##### 解决(其实还没解决)
```
lsmod |grep conntrack
modprobe ip_conntrack
lsmod |grep conntrack
```
