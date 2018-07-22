## 1. 生产环境负载均衡集群系统架构设备选购案例
### 1-1. 依据
1. 价格
2. 性能
3. 冗余

### 1-2. 负载均衡器
负载均衡器主要处理请求分发,对CPU/内存有要求,而磁盘大小不重要
+ LVS1 主: DELL R610 1U, CPU E5606\*2, 4G\*2 内存 硬盘:SAS 146G\*2 RAID1
+ LVS2 主: DELL R610 1U, CPU E5606\*2, 4G\*2 内存 硬盘:SAS 146G\*2 RAID1

### 1-3. Web层
+ WWW 主站1 业务(两台):DELL R710, CPU E5606\*2, 4G\*4 内存 硬盘:SAS 300G\*2 RAID0
+ WWW 主站2 业务(两台):DELL R710, CPU E5606\*2, 4G\*4 内存 硬盘:SAS 300G\*2 RAID0

### 1-4. 数据库层
主从库之间磁盘大小要一致;性能和维护这种考虑,从库也可以考虑做RAID5等.根据实际情况,也可以考虑多实例
+ MySQL 主库1-1 :DELL R710, CPU E5606\*2, 4G\*8 内存 硬盘:SAS 600G\*6(或146G\*6) RAID10
+ MySQL 主库1-2 :DELL R710, CPU E5606\*2, 4G\*8 内存 硬盘:SAS 600G\*6(或146G\*6) RAID10
+ MySQL 从库1-1 :DELL R710, CPU E5606\*2, 4G\*4 内存 硬盘:SAS 15k 600G\*4 RAID0/RAID5
+ MySQL 从库1-2 :DELL R710, CPU E5606\*2, 4G\*4 内存 硬盘:SAS 15k 600G\*4 RAID0/RAID5
+ MySQL 从库2-1 :DELL R710, CPU E5606\*2, 4G\*4 内存 硬盘:SAS 15k 600G\*4 RAID0/RAID5
+ MySQL 从库2-2 :DELL R710, CPU E5606\*2, 4G\*4 内存 硬盘:SAS 15k 600G\*4 RAID0/RAID5


### 1-5. 存储层
#### 1-5-1. 数据备份
考虑容量和冗余,性能要求不高
+ DELL R610, CPU E5606\*2, 16G 内存 硬盘:SATA 10k 2T\*4 可以不做RAID交叉备份
+ DELL R710, CPU E5606\*2, 16G 内存 硬盘:SATA 10k 2T\*6 RAID5 做RAID5是折中方案

#### 1-5-2. 共享存储
+ NFS1:DELL R710, CPU E5606\*2, 16G 内存 硬盘:SAS 15k 600G\*6 RAID10/RAID5/RAID0
+ NFS2:DELL R710, CPU E5606\*2, 16G 内存 硬盘:SAS 15k 600G\*6 RAID10/RAID5

### 1-6. 监控管理网关层
也可以不单独购买,而和存储层的服务器共用.虽然很重要,但只是对内网很重要,所以不用特别高的性能
+ DELL R610, CPU E5606\*1, 8G 内存 硬盘:146G\*2 RAID1
### 1-7. cache服务器
#### 1-7-1. 工具
+ Web缓存:squid,nginx,varnish
+ 数据库缓存:memcache,redis,ttserver
#### 1-7-2. 方案
+ Memcache:内存大即可,无需磁盘空间
+ 持久化系统如redis,ttserver:内存和磁盘兼顾

### 1-8. 网络设备
H3C和Dlink相对便宜
+ CISCO
+ H3C
+ DLINK


