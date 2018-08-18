
## 1. 安装
### 1-1. 安装版本1
1. 安装软件包
2. 创建可信池(主机名/IP地址)
    + glusterfs的多个节点没有master,都是对等的
3. 创建卷(分布式卷/)
    + 分布式卷:将文件写到不同的brick(可以理解为卷)
```
#####################################################################
# 10.0.0.{7,8}
#####################################################################
yum install -y centos-release-gluster
yum install -y glusterfs-server 

systemctl enable glusterd
systemctl start glusterd
systemctl status glusterd

systemctl stop firewalld
systemctl stop iptables
setenforce 0
vi /etc/selinux/config 
#################################################################
SELINUX=disabled
#################################################################

#####################################################################
# 10.0.0.7或10.0.0.8,任何一个都可以,只需要单方面probe就行了
#####################################################################
gluster peer probe 10.0.0.8
gluster peer status # 2台主机上都看到结果

#####################################################################
# 10.0.0.7
#####################################################################
mkdir -p /data/exp1
mkdir -p /data/exp3
mkdir -p /data/exp5




#####################################################################
# 10.0.0.8
#####################################################################
mkdir -p /data/exp2
mkdir -p /data/exp4
mkdir -p /data/exp6






#####################################################################
# 10.0.0.7/8
#####################################################################
# 他们的建议是用新的磁盘来做
gluster volume create test-volume 10.0.0.7:/data/exp1 10.0.0.8:/data/exp2 force # 分布式卷
gluster volume create repl-volume replica 2 transport tcp 10.0.0.7:/data/exp3 10.0.0.8:/data/exp4 force # 复制卷
gluster volume create raid0-volume stripe 2 transport tcp 10.0.0.7:/data/exp5 10.0.0.8:/data/exp6 force # 条带卷

gluster volume info # 可以看到是否启动了
gluster volume info repl-volume

gluster volume start test-volume
gluster volume start repl-volume
gluster volume start raid0-volume

# yum install -y glusterfs-client



#####################################################################
# 10.0.0.7
#####################################################################
mkdir /mnt/distributed /mnt/replicated /mnt/striped
mount.glusterfs 10.0.0.7:/test-volume /mnt/distributed # 任一个节点都可以
mount.glusterfs 10.0.0.7:/repl-volume /mnt/replicated # 任一个节点都可以
mount.glusterfs 10.0.0.7:/raid0-volume /mnt/striped # 任一个节点都可以

df -h # 可以看到复制卷会少一半的空间,而分布式卷和条带卷都是原来的空间大小
#----------------------------------------------------------------------
# Filesystem              Size  Used Avail Use% Mounted on
# 10.0.0.7:/test-volume    18G  2.4G   16G  14% /mnt/distributed
# 10.0.0.7:/repl-volume   8.8G  1.2G  7.6G  14% /mnt/replicated
# 10.0.0.7:/raid0-volume   18G  2.4G   16G  14% /mnt/striped
#----------------------------------------------------------------------

echo 1 > /mnt/distributed/test1.txt
tree /data/* # 2个节点都可以查看,可以发现exp1下有test1.txt了,而10.0.0.8的exp2下没有test1.txt
#----------------------------------------------------------------------
# /data/exp1
# └── test1.txt
#----------------------------------------------------------------------


echo 1 > /mnt/replicated/test2.txt
tree /data/* # 可以看到复制卷会在2个节点上都写test2.txt这个文件
#----------------------------------------------------------------------
# /data/exp4
# └── test2.txt
# /data/exp3
# └── test2.txt
#----------------------------------------------------------------------



man bash > /mnt/striped/striped.txt # 由于条带卷更适合存储大型文件,所以这里直接拿大型文件测试
tree /data/* # 可以看到条带卷在2个节点上都写了文件
#----------------------------------------------------------------------
# /data/exp5
# └── striped.txt
# /data/exp6
# └── striped.txt
#----------------------------------------------------------------------


wc -l /data/exp5/striped.txt # 10.0.0.7
#----------------------------------------------------------------------
# 2324 /data/exp5/striped.txt
#----------------------------------------------------------------------



wc -l /data/exp6/striped.txt # 10.0.0.8
#----------------------------------------------------------------------
# 1732 /data/exp6/striped.txt
#----------------------------------------------------------------------



man bash | wc -l # 可以看到对于条带卷,2个节点上的数据合起来才是最终的数据
#----------------------------------------------------------------------
# 4056
#----------------------------------------------------------------------

```







### 1-2. 安装版本2
1. 环境说明
    1. GlusterFS节点1
        1. eth0:10.0.0.7
        2. eth1:10.0.1.7
        3. 挂载1个磁盘/dev/sdb
    2. GlusterFS节点2
        1. eth0:10.0.0.8
        2. eth1:10.0.1.8
        3. 挂载1个磁盘/dev/sdb
    3. GlusterFS节点3
        1. eth0:10.0.0.9
        2. eth1:10.0.1.9
        3. 挂载1个磁盘/dev/sdb
    4. 系统环境
    ```
    [root@glusterfs01 ~]# cat /etc/redhat-release 
    CentOS Linux release 7.5.1804 (Core) 
    [root@glusterfs01 ~]# uname -r
    3.10.0-862.el7.x86_64
    [root@glusterfs01 ~]# uname -m
    x86_64
    ```
2. 下载软件包
    1. 下载glusterfs,api,cli,fuse,libs,server(客户端只需要glusterfs,libs和fuse)
```
##########################################################################
# 确认环境
##########################################################################
ifconfig
fdisk -l


##########################################################################
# 关闭防火墙和SeLinux
##########################################################################
systemctl stop iptables
systemctl disable iptables
systemctl stop firewalld
systemctl disable iptables
setenforce 0
sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config


##########################################################################
# 安装依赖包
##########################################################################
yum install -y centos-release-gluster
yum install -y wget rpcbind libaio lvm2-devel psmisc attr userspace-rcu-devel gcc

mkdir ~/tools
cd ~/tools


##########################################################################
# 安装glusterfs
##########################################################################
wget https://buildlogs.centos.org/centos/7/storage/x86_64/gluster-4.1/glusterfs-4.1.2-1.el7.x86_64.rpm
wget https://buildlogs.centos.org/centos/7/storage/x86_64/gluster-4.1/glusterfs-api-4.1.2-1.el7.x86_64.rpm
wget https://buildlogs.centos.org/centos/7/storage/x86_64/gluster-4.1/glusterfs-cli-4.1.2-1.el7.x86_64.rpm
wget https://buildlogs.centos.org/centos/7/storage/x86_64/gluster-4.1/glusterfs-fuse-4.1.2-1.el7.x86_64.rpm
wget https://buildlogs.centos.org/centos/7/storage/x86_64/gluster-4.1/glusterfs-libs-4.1.2-1.el7.x86_64.rpm
wget https://buildlogs.centos.org/centos/7/storage/x86_64/gluster-4.1/glusterfs-server-4.1.2-1.el7.x86_64.rpm
wget https://buildlogs.centos.org/centos/7/storage/x86_64/gluster-4.1/glusterfs-client-xlators-4.1.2-1.el7.x86_64.rpm

rpm -ivh glusterfs-libs-4.1.2-1.el7.x86_64.rpm 
rpm -ivh glusterfs-4.1.2-1.el7.x86_64.rpm 
rpm -ivh glusterfs-cli-4.1.2-1.el7.x86_64.rpm 
rpm -ivh glusterfs-client-xlators-4.1.2-1.el7.x86_64.rpm
rpm -ivh glusterfs-api-4.1.2-1.el7.x86_64.rpm 
rpm -ivh glusterfs-fuse-4.1.2-1.el7.x86_64.rpm 
rpm -ivh glusterfs-server-4.1.2-1.el7.x86_64.rpm 


##########################################################################
# 安装测试工具
##########################################################################
wget https://www.atoptool.nl/download/atop-2.3.0-1.el7.x86_64.rpm
wget http://ftp.tu-chemnitz.de/pub/linux/dag/redhat/el7/en/x86_64/rpmforge/RPMS/fio-2.1.10-1.el7.rf.x86_64.rpm
wget http://rpmfind.net/linux/dag/redhat/el7/en/x86_64/dag/RPMS/iozone-3.424-2.el7.rf.x86_64.rpm
rpm -ivh atop-2.3.0-1.el7.x86_64.rpm 
rpm -ivh fio-2.1.10-1.el7.rf.x86_64.rpm 
rpm -ivh iozone-3.424-2.el7.rf.x86_64.rpm 
wget https://raw.githubusercontent.com/Andiry/postmark/master/postmark-1.53.c
gcc -o postmark postmark-1.53.c
cp postmark /usr/local/bin/
postmark # quit
atop
iozone
fio


##########################################################################
# 域名解析
##########################################################################
cat >> /etc/hosts <<EOF
10.0.0.7 glusterfs01
10.0.0.8 glusterfs02
10.0.0.9 glusterfs03
EOF
ping glusterfs02


##########################################################################
# 时间同步
##########################################################################
yum install -y ntpd
vi /etc/ntp.conf
##########################################################################
server 0.centos.pool.ntp.org iburst
server 1.centos.pool.ntp.org iburst
server 2.centos.pool.ntp.org iburst
server 3.centos.pool.ntp.org iburst
server glusterfs01
##########################################################################
systemctl restart ntpd
ntpdate glusterfs01
date

##########################################################################
# 挂载磁盘
##########################################################################
mkfs.ext4 /dev/sdb 
mkdir /brick1
mount /dev/sdb /brick1/



```


## 2. 卷
### 2-1. 创建分布式复制卷
创建分布式复制卷的时候,不同brick的功能和创建命令里的顺序有关
```
mkdir -p /exp1
mkdir -p /exp2
gluster volume create dr-volume replica 2 transport tcp 10.0.0.7:/exp1 10.0.0.8:/exp1 10.0.0.7:/exp2 10.0.0.8:/exp2 force # 分布式卷

gluster volume info dr-volume # 2 * 2 = 4

gluster volume start dr-volume
mkdir /mnt/distributed-striped
mount.glusterfs 10.0.0.7:/dr-volume /mnt/distributed-striped # 任一个节点都可以

df -h

man bash > /mnt/distributed-striped/test1.txt
man bash > /mnt/distributed-striped/test2.txt
man bash > /mnt/distributed-striped/test3.txt

tree /exp* # 发现2个节点上都各自拥有1份testx.txt文件.这样就能保证1个节点挂了而数据全在
```
### 2-2. 添加brick
1. 给分布式卷添加brick
    1. 创建brick(目录)
    2. 将该brick添加到分布式卷里
    3. 重新均衡下分布式卷
```
mkdir /data/addition
gluster volume add-brick test-volume 10.0.0.7:/data/addition force

gluster volume status test-volume # 如果brick的Online那里写着Y说明就正确了

for i in `seq 100`; do echo 1 > /mnt/distributed/$i;done

tree /data/* # 会发现新添加的brick里面没有数据,这是因为添加新的brick一定要重新均衡一下

gluster volume rebalance test-volume start # 会很费时间

tree /data/* # 会发现/data/addition目录已经有数据了
```

### 2-3. 移除brick
数据可能会丢失.但是挂载的目录里的文件可能会都在,不过在GlusterFS的目录里就已经丢失数据了
```
gluster volume remove-brick test-volume 10.0.0.7:/data/addition force
#gluster volume rebalance test-volume start # 会很费时间
```


