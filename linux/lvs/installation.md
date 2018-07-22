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
#### ipvsadm

#### 清空当前列表
```
ipvsadm -C
```
#### 设置tcp,tcpfin,udp的超时时间(可选)
```
ipvsadm --set 30 5 60
```
#### 添加virtual server
设置`172.19.28.82:80`为虚拟节点
+ 最好写明端口
+ 这里写的`-p 22`不是必需的
```
ipvsadm -A -t 172.19.28.82:80 -s rr -p 20
```
#### 添加real server
在`172.19.28.82:80`的虚拟节点上添加real server `172.19.28.8x:80`,工作模式为DR,权重为1
```
ipvsadm -a -t 172.19.28.82:80 -r 172.19.28.83:80 -g -w 1
ipvsadm -a -t 172.19.28.82:80 -r 172.19.28.84:80 -g -w 1
```
#### 检查是否添加成功
```
ipvsadm -L -n
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
