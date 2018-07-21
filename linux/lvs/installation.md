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
### 2. 常见问题
#### 2-1. 
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
