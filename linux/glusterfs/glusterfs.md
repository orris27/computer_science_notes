## 1. 安装
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
### 2-1. 添加brick
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
