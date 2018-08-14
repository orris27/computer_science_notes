## 1. EIGRP
可以理解为加强版的RIP协议.主要特点是可以近距离收缩,不发送定期的路由更新信息以减少带宽的占用
### 1-1. 语法

1. `router eigrp 10`:表示修改eigrp为10的配置
2. `network 192.168.110.0`:给eigrp添加`192.168.110.0`这个网段
3. 

network 192.168.120.0

### 1-2. 与RIP比较

1. EIGRP优先级高于RIP,所以通过EIGRP学习到的路由会清除掉RIP学习到的路由.优先级: 0:直连C 1:静态路由S 90:EIGRP学习到的 120:RIP学习到的路由D

2. EIGRP是和RIP类似的协议,只不过只会广播给部分路由器

3. EIGRP和RIP协议里的network可以类似于路由合并一样写多个路由.比如101.1.0.0和101.2.0.0都是A类网段,可以简写成101.0.0.0;而131.7.1.0,131.8.1.0和131.7.2.0都是B类网段,只能简写成131.7.0.0和131.8.0.0.(需要根据A/B类网段来区分)

   

## 2. 虚拟机网络

### 2-1. 基本原理

虚拟机里1个VMnet就是1个交换机.所以ifconfig可以看到vmnet1和vmnet8,这表示和这些虚拟交换机连接了.

- 虚拟交换机在物理机里面<=>物理机和这些虚拟交换机相连接
- `Connect a host virtual adapter to this network`表示给这个虚拟交换机添加一个虚拟网卡.这样在物理机上就会多出响应的vmnetx的网卡,这个网卡就会连接到这个虚拟交换机
- 我们将虚拟机放到vmnetx<=>把这些虚拟机和这个vmnetx的虚拟交换机相连接
- 一个vmnetx其实对应1个网段,一般来说互相独立

### 2-2. vmnet0

vmnet0是和自己的物理机处于同样的网段.为桥接模式.

- 即使自己的物理机IP地址设置错误了,虚拟机却还是能通信
- 虚拟机仍然走物理机的有线/无线网卡
- 可以设置虚拟机桥接到哪个网卡(默认是Auto)
- 网线拔了或者没有无线网络,虚拟机也是上不了网的

### 2-3. custome

Custom里设置放到哪个交换机里



## 3. 思科设备

里面有操作系统,叫网际操作系统(IOS).通过IOS来实现功能

### 3-1. 交换机

网桥比如有2个接口,然后数据从哪个接口来,就记下<接口,MAC地址>.端口独享带宽

MAC地址:dynamic

#### 3-1-1. 安全

##### 3-1-1-1. 限定数量

限定数量=>允许连接该交换机的计算机/其他设备的数量

1. 进入接口的配置
2. 限制在access模式下
3. 启动交换机安全
4. 如果违反的话怎么操作
5. 限定数量的话是什么值

```
config terminal
interface fastEthernet 0/4
switchport mode access
switchport port-security
switchport port-security violation shutdown
switchport port-security maximum 2
```

##### 3-1-1-2. 绑定MAC

1. 在路由器上启动安全
2. 先ping获得MAC地址
3. 绑定当前的MAC地址

```
config terminal 
interface fastEthernet 0/1 - 24
switchport mode access
switchport port-security
switchport port-security violation shutdown
exit
#+++++++++++++++++++++++++++++++++++
# 执行各种ping操作,获得MAC地址
#+++++++++++++++++++++++++++++++++++
interface fastEthernet 0/1 - 24
switchport port-security mac-address sticky
```

#### 3-1-2. 生成树协议

1. 产生原因

   1. 分级交换机(所有省份的交换机都接到中国的交换机,每个省份下的所有市的交换机都接到这个省份的交换机里)没有高可用=>高级交换机弄2个=>环路=>广播风暴(一产生广播就停止不下来)

2. 生成树协议会阻断接口(橙色).默认就会启用生成树协议.树是没有环的数据结构.

   1. 选择根交换机(优先级+MAC地址)
   2. 选择根端口
   3. 每个网线端选择一个指定端口
   4. 剩下的为阻断端口

   ```
   config terminal
   show spanning tree
   ```

### 3-2. 路由器
连接2个不同网络的设备就是路由器.三层交换机也可以理解是路由器(通过上面绑定多个不同网段内的网卡,其实也是实现了路由的功能)
1. 路由器分类
   1. 模块化路由器:可以扩展
      1. 在GNS3上slot就是扩展.PA-4E表示可以接4个以太网接口
      2. E表示普通以太网接口
      3. FE表示快速以太网接口
      4. T表示广域网接口.如PA-4T+
   2. 非模块化路由器:能插取的端口有限
2. 接口
   1. 广域网接口:距离远,传输速度慢
   2. 以太网接口:距离近,传输速度快

​		也有RAM,ROM

#### 3-2-1. 配置路由器:

​	用COM线+COM线转USB连接了路由器和笔记本电脑后,在笔记本上的SecureCRT上连接Serial+COM12+9600的比特率可以连接路由器

​	GNS3

语法

1. `interface ethernet 1/0`表示第1个模块的第0个接口.该路由器为模块化路由器.
   - 这里的第几个模块指的是第几个插槽
   - GNS3中有第0个插槽,所以有第0个模块
2. `config terminal`进入配置的终端
   1. `RI(config)#`配置全局的参数.
   2. `^Z`/`exit`退出配置的终端
   3. `interface ethernet 1/0`进入配置指定的以太网接口的模式
      1. `ip address 10.0.0.2 255.255.255.0`表示配置IP地址
      2. `no shutdown`表示启动配置
   4. `interface serial 2/0`进入配置指定的广域网接口的模式
      1. `clock rate 64000`设置时钟频率.运营商通过时钟频率来限制带宽.
         - 定义时钟频率的一方叫做DCE,接收时钟频率的乙方叫做DTE.
         - 实际生产环境是运营商定义时钟频率的.我们只需要被动接收时钟频率
         - 网线的两端分别是DCE,DTE
         - 用DCE端连接的设备需要定义时钟频率
         - 模拟环境的话我们随便在广域网的任何一端顶一个时钟频率就行了.定义的语句就是`clock rate`
3. `enable`表示进入特权模式
4. `disable`表示退出特权模式
5. `show`
   1. `show version`查看当前路由器版本
   2. `show running-config`
   3. `show interface`
6. `?`查看帮助

```
config t
interface ethernet 1/0 # 表示
ip address 10.0.0.2 255.255.255.0
no sh
```

## 4. VLAN

虚拟局域网.将物理上不连接在同一个交换机上的主机连接成一个网络/广播域.通常是根据业务来分成一个VLAN

### 4-1. 创建VLAN

1. 在交换机上创建2个VLAN<=>交换机一刀两断成为2个交换机并且接到1个路由器上
2. 在2个交换机上根据业务创建2个VLAN.交换机a和交换机b都切割成2个.然后用2根线连在一起,这样就构成了2个VLAN.
   - 干道链路技术

### 4-2. 干道链路技术
跨交换机通过连接干道链路实现VLAN创建.干道链路技术可以跑多个VLAN的数据.1根干道链路可以通过标签来区分不同VLAN,从而我们不用2根线而用1个干道链路就能实现
#### 4-2-1. 原理
当vlan1发出广播后,同一个交换机里只有vlan1的主机会发出响应.经过干道链路后,干道链路给这个信号加上标签,表明它是vlan1的信号.这样一来,当到达其他交换机后,这个交换机可以通过这个vlan1的标签来决定主机.

### 4-3. 利用Packet Tracer配置VLAN
1. 设备选择
    1. PC选择Generic,为`192.168.0.2`,`192.168.0.3`,`192.168.1.2`和`192.168.1.3`
    2. Switch选择2950T-24
    3. Multilayer Switch选择3560-24PS
2. 连线
    1. PC和Switch用Straight-Through
    2. Switch和Multilayer Switch用Cross-Over
3. 在Switch上配置路由
    1. 左击Switch,选中CLI
    2. 我们让1-12在vlan1,而13-24为vlan2
    3. 让`192.168.1.2`的PC连接第一个交换机的0/13接口
    ```
    Switch>enable
    Switch#configure terminal
    Switch(config)#interface range fastEthernet 0/13 - 24
    Switch(config-if-range)#switchport mode access
    Switch(config-if-range)#switchport access vlan 2
    % Access VLAN does not exist. Creating vlan 2 % 也可以先创建vlan 2,只要输入vlan 2 的命令就会创建了
    Switch(config-if-range)#exit
    Switch(config)#exit
    Switch#show vlan
    ```
4. 在交换机和三层交换机之间配置干道链路
    1. 选中普通交换机
    ```
    enable
    interface gigabitEthernet 1/1
    switchport mode ?
    switchport mode trunk
    ```
    
    
    




## 5. ens33 => eth0
1. 修改`/etc/sysconfig/network-scripts/ifcfg-ens33`中的Name和Device
```
sed -i 's/ens33/eth0/g' /etc/sysconfig/network-scripts/ifcfg-ens33
```

2. 重命名`ifcfg-ens33`
```
mv /etc/sysconfig/network-scripts/ifcfg-ens33 /etc/sysconfig/network-scripts/ifcfg-eth0
```



3. 修改`/etc/defualt/grub`
+ 加入"net.ifnames=0 biosdevname=0"到GRUB_CMDLINE_LINUX变量
```
sed -i '/GRUB_CMDLINE_LINUX/s/"$/\ net.ifnames=0 biosdevname=0"/g' /etc/default/grub
```


4. 运行命令`grub2-mkconfig -o /boot/grub2/grub.cfg`来重新生成GRUB配置并更新内核参数
```
grub2-mkconfig -o /boot/grub2/grub.cfg
```

5. 重启系统
```
reboot
```

6. 验证
```
ifconfig
```
