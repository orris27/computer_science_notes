## 1. PHP
1. `error while loading shared libraries: libpcre.so.1: cannot open shared object file: No such file or directory`
解决办法:  
指定库或者在安装xx的时候指定目录   
下面给出的是指定库的方法  
64为的是要给出lib64
```
find / -name libpcre.so* # 先尝试找到这个文件
# /usr/local/lib/libpcre.so.1 # 如果找到这个文件了
echo '/usr/local/lib' >> /etc/ld.so.conf # 将这个路径放到/etc/ld.so.conf里面
# echo '/usr/local/lib64' >> /etc/ld.so.conf
ldconfig # 生效该配置文件
```
## 2. SaltStack
### 2-1. etcd版本的SaltStack推送haproxy配置文件
> https://github.com/orris27/orris/blob/master/linux/saltstack/etcd.md

1. 我在etcd设置好后执行`salt '*' pillar.items`却没有获取到设置的<key,valule>

- 在master的日志文件中查找,如`tail /var/log/salt/master`

2. 不能导入etcd

```
2018-08-01 19:50:39,942 [salt.pillar      ][ERROR   ][11797] Failed to load ext_pillar etcd: (unable to import etcd, module most likely not installed)
Traceback (most recent call last):
  File "/usr/lib/python2.7/site-packages/salt/pillar/__init__.py", line 538, in ext_pillar
    key)
  File "/usr/lib/python2.7/site-packages/salt/pillar/__init__.py", line 508, in _external_pillar_data
    val)
  File "/usr/lib/python2.7/site-packages/salt/pillar/etcd_pillar.py", line 97, in ext_pillar
    client = salt.utils.etcd_util.get_conn(__opts__, profile)
  File "/usr/lib/python2.7/site-packages/salt/utils/etcd_util.py", line 81, in get_conn
    '(unable to import etcd, '
CommandExecutionError: (unable to import etcd, module most likely not installed)
```

- `sudo pip install etcd`

3. module里面没有Client属性

```
2018-08-01 20:02:26,145 [salt.pillar      ][ERROR   ][14730] Failed to load ext_pillar etcd: 'module' object has no attribute 'Client'
Traceback (most recent call last):
  File "/usr/lib/python2.7/site-packages/salt/pillar/__init__.py", line 538, in ext_pillar
    key)
  File "/usr/lib/python2.7/site-packages/salt/pillar/__init__.py", line 508, in _external_pillar_data
    val)
  File "/usr/lib/python2.7/site-packages/salt/pillar/etcd_pillar.py", line 97, in ext_pillar
    client = salt.utils.etcd_util.get_conn(__opts__, profile)
  File "/usr/lib/python2.7/site-packages/salt/utils/etcd_util.py", line 78, in get_conn
    return etcd.Client(host, port)
AttributeError: 'module' object has no attribute 'Client'
```

- 错误地安装了不应该安装的`etcd`而没有/覆盖了本应安装`python-etcd`

  1. 我的Ubuntu直接安装`python-etcd`,而不安装`pip install etcd`时,就能执行`import etcd`和`etcd.Client()`
  2. 不要安装`pip install etcd`
  3. 如果安装了`python-etcd`还是显示这个,说明`urllib3`应该更换(并且会出现第4条错误)

  ```
  sudo pip uninstall etcd
  sudo pip uninstall urllib3
  sudo pip install urllib3 --upgrade
  python
  ###################
  import etcd
  etcd.Client() # 只要没有提示说'module' object has no attribute 'Client',就是正确了
  ###################
  
  ```

4. `cannot import name UnrewindableBodyError`

- 如果只安装了`python-etcd`,而没有安装`etcd`的话,会出现该错误提示(注意:这个是正确的步骤,千万不要安装`etcd`)
- 解决方法:重新卸载并安装`urllib3`

```
sudo pip uninstall urllib3
sudo pip install urllib3 --upgrade
```
5. 推送完后启动haproxy失败
    使用`systemctl status haproxy`发现如下错误信息
    ```
    Aug 02 00:26:40 linux-node1.example.com haproxy[27178]: [ ALL] send-proxy-v2 [dflt_ok]
    Aug 02 00:26:40 linux-node1.example.com haproxy[27178]: [ ALL] source <arg> [dflt_ok]
    Aug 02 00:26:40 linux-node1.example.com haproxy[27178]: [ ALL] stick [dflt_ok]
    Aug 02 00:26:40 linux-node1.example.com haproxy[27178]: [ ALL] track <arg> [dflt_ok]
    Aug 02 00:26:40 linux-node1.example.com haproxy[27178]: [ TCP] tcp-ut <arg> [dflt_ok]
    Aug 02 00:26:40 linux-node1.example.com haproxy[27178]: [ALERT] 213/002640 (27180) : Error(s) found in configuratio....cfg
    Aug 02 00:26:40 linux-node1.example.com haproxy[27178]: [ALERT] 213/002640 (27180) : Fatal errors found in configuration.
    Aug 02 00:26:40 linux-node1.example.com systemd[1]: Unit haproxy.service entered failed state.
    Aug 02 00:26:40 linux-node1.example.com haproxy[27178]: Errors found in configuration file, check it with 'haproxy check'.
    Aug 02 00:26:40 linux-node1.example.com systemd[1]: haproxy.service failed.
    Hint: Some lines were ellipsized, use -l to show in full.
    ```
    1. 进一步使用haproxy的check功能检查,即`haproxy -c -f /etc/haproxy/haproxy.cfg`,发现报错`[ALERT] 213/003053 (27918) : parsing [/etc/haproxy/haproxy.cfg:36] : 'server web-node1' unknown keyword 'fail'. Registered keywords :`
    + 说明`fail`写错=>实际上为`fall`
6. 执行完SaltStack的状态文件后,为什么我的haproxy服务是停止的,而必须要手动开启?
+ 因为我只对一个服务器(linux-node1)执行状态文件了=>如果我对两个服务器都执行状态文件,那么haproxy服务器最终就是开启的


7. `No package salt-master available.`
> 因为没有安装epel源,所以要安装
```
wget -O /etc/yum.repos.d/epel.repo http://mirrors.aliyun.com/repo/epel-7.repo
```



## 3. VMware Workstation
### 3-1. 安装VMware Workstation
1. `(vmware-installer.py:10466): Gtk-WARNING **: Unable to locate theme engine in module_path: "murrine"`
    1. 安装组件
    ```
    sudo apt-get install murrine-themes
    sudo apt-get install gtk2-engines-murrine
    sudo apt-get install libgtkmm-2.4-1c2a
    sudo apt-get install libgtkmm-2.4-dev
    sudo apt-get install libcanberra-gtk-module:i386
    sudo apt-get install gnome-tweak-tool
    sudo apt-get install gksu
    ```
    2. 最后使用:`gksudo bash ./VMware-Workstation-Full-12.1.1-3770994.x86_64.bundle`安装
### 3-2. 安装CentOS
1. 此主机支持 Intel VT-x，但 Intel VT-x 处于禁用状态
```
This host supports Intel VT-x, but Intel VT-x is disabled.
Intel VT-x might be disabled if it has been disabled in the BIOS/firmware settings or the host has not been power-cycled since changing this setting.
(1) Verify that the BIOS/firmware settings enable Intel VT-x and disable 'trusted execution.'
(2) Power-cycle the host if either of these BIOS/firmware settings have been changed.
(3) Power-cycle the host if you have not done so since installing VMware Workstation.
(4) Update the host's BIOS/firmware to the latest version.
This host does not support "Intel EPT" hardware assisted MMU virtualization.
Module 'CPUIDEarly' power on failed.
Failed to start the virtual machine.
```
+ Intel的虚拟技术处于关闭状态=>在BIOS中开启intel virtual technology
    1. 进入BIOS
    2. 进入CPU Configuration
    3. 将Intel虚拟技术选择启动
    
2. 需要安装VMware Tools
```
The VMware Tools package is not running in this virtual machine. The package might be necessary for the guest operating system to run at resolutions higher than 640x480 with 16 colors. The package provides significant performance benefits as well. To install it, choose VM > Install VMware Tools… after the guest operating system starts.
VMware Workstation can remind you to install the VMware Tools package when you power on the VM. Select OK to enable the reminder.
```
+ 不用安装(放大字体=>ssh客户端)

https://blog.csdn.net/zhaihaifei/article/details/56674655

## 4. Cobbler
### 4-1. 安装
1. yum中没有找到`cobbler`
+ `yum -y install epel-release`,更新yum的epel
2. Protocol Error
```
httpd does not appear to be running and proxying cobbler, or SELinux is in the way. Original traceback:
Traceback (most recent call last):
  File "/usr/lib/python2.7/site-packages/cobbler/cli.py", line 251, in check_setup
    s.ping()
  File "/usr/lib64/python2.7/xmlrpclib.py", line 1233, in __call__
    return self.__send(self.__name, args)
  File "/usr/lib64/python2.7/xmlrpclib.py", line 1591, in __request
    verbose=self.__verbose
  File "/usr/lib64/python2.7/xmlrpclib.py", line 1273, in request
    return self.single_request(host, handler, request_body, verbose)
  File "/usr/lib64/python2.7/xmlrpclib.py", line 1321, in single_request
    response.msg,
ProtocolError: <ProtocolError for 127.0.0.1:80/cobbler_api: 503 Service Unavailable>
```
+ 需要关闭SELinux才能正常执行
    1. 查看SELinux状态,`getenforce`
    2. 关闭SElinux,`setenforce 0`


3. `cobbler PXE-E53:No boot filename received`
+ 连不上网
    1. 关闭Cobbler服务器上的防火墙,iptables和firewalld
    2. 虚拟机使用桥接模式
    3. 是否下面链接里的所有服务都启动了,如dhcp一定要启动
    > https://github.com/orris27/orris/blob/master/linux/cobbler/cobbler.md
    4. 也有尝试下面网站的方法,修改了cobbler的配置文件并且安装了dnsmasq
    > https://blog.csdn.net/zhaihaifei/article/details/56674655


4. `you have specified that the group base should be installed`
+ 我使用的CentOS是minimal版本,没有base等包=>在cfg文件里删除这些包就可以了


5. `unable to open input kickstart file: curl#37 "Couldn't open file /tmp/pre_install_network_config"`

6. 浏览器里输入`http://192.168.1.2/cobbler_web`时返回`403 Forbidden`
+ 输入`https://192.168.1.2/cobbler_web`,这里一定是`https`
+ 如果浏览器阻止的话,选高级,强行进入


## 5. Elasticsearch

1. `max file descriptors [4096] for elasticsearch process is too low, increase to at least [65536].`
+ 修改`/etc/security/limits.conf`只要修改就生效了,不用类似于内核参数一样修改完文件后还要执行`sysctl -p`或者修改配置文件后重启服务等
```
vim /etc/security/limits.conf
################################################################
* soft nofile 65536
* hard nofile 65536
* soft nproc 2048
* hard nproc 4096
################################################################
```
2. `memory locking requested for elasticsearch process but memory is not locked`
    > 原因: 发生系统swapping的时候ES节点的性能会非常差，也会影响节点的稳定性。所以要不惜一切代价来避免swapping.要么是bootstrap.memory_lock: true这个没设置，要么就是max locked memory这个没配置么
    1. 尝试
    ```
    
    # 尝试修改/etc/security/limits.conf => 结果还是报同样的错误
    vim /etc/security/limits.conf 
    ################################################################
    *   soft　　memlock　　unlimited
    *   hard　　memlock　　unlimited
    ################################################################


    # 尝试修改/etc/systemd/system.conf => 结果还是报同样的错误
    vim /etc/systemd/system.conf
    ################################################################
    DefaultLimitNOFILE=65536
    DefaultLimitNPROC=32000
    DefaultLimitMEMLOCK=infinity
    ################################################################


    # 根据https://blog.csdn.net/gebitan505/article/details/54709515,尝试在配置文件里取消注释 => 结果还是报同样的错误
    vim /etc/elasticsearch/elasticsearch.yml
    ################################################################
    discovery.zen.ping.unicast.hosts: ["192.168.56.10"]
    discovery.zen.minimum_master_nodes: 3
    ################################################################



    # 尝试修改/etc/security/limits.conf => 结果还是报同样的错误+第3个错误,即threads不够用
    vim /etc/security/limits.conf 
    ################################################################
    elasticsearch                soft nofile 65536
    elasticsearch                hard nofile 65536
    elasticsearch                soft nproc 2048
    elasticsearch                hard nproc 4096
    elasticsearch                soft　　memlock　　unlimited
    elasticsearch                hard　　memlock　　unlimited
    ################################################################

    # 尝试修改/etc/security/limits.conf,注释里面刚好有memlock unlimited,我就取消这个注释,发现执行su -s /bin/sh -c "/usr/share/elasticsearch/bin/elasticsearch" elasticsearch成功,但是执行systemctl start elasticsearch失败,还是报memory is not locked的错误
    vim /etc/security/limits.conf 
    ################################################################
    #*               soft    core            0
    #*               hard    rss             10000
    #@student        hard    nproc           20
    #@faculty        soft    nproc           20
    #@faculty        hard    nproc           50
    #ftp             hard    nproc           0
    #@student        -       maxlogins       4
    *                hard    memlock         unlimited
    *                soft    memlock         unlimited
    ################################################################
    ```
    2. 最终解决方案
        1. 修改limits的参数(建议关闭防火墙和selinux等)
        2. 使用yum安装时自带的`elasticsearch`用户启动
    ```
    vim /etc/security/limits.conf 
    ################################################################
    #*               soft    core            0
    #*               hard    rss             10000
    #@student        hard    nproc           20
    #@faculty        soft    nproc           20
    #@faculty        hard    nproc           50
    #ftp             hard    nproc           0
    #@student        -       maxlogins       4
    *                hard    memlock         unlimited # 这个不要自己输入,而是取消注释!!(也可能是和顺序有关)
    *                soft    memlock         unlimited
    *                soft nofile 65536
    *                hard nofile 65536
    *                soft nproc 2048
    *                hard nproc 4096
    ################################################################
    
    su -s /bin/sh -c "/usr/share/elasticsearch/bin/elasticsearch" elasticsearch
    #su -s /bin/sh -c "/usr/share/elasticsearch/bin/elasticsearch -d" elasticsearch # -d在后台运行
    ```

3. `max number of threads [2048] for user [elasticsearch] is too low, increase to at least [4096]`
```
vim /etc/security/limits.conf # 上面的修改成下面的,即soft nproc改成4096 => 结果又报第2个错误,即memory is not locked
################################################################
elasticsearch                soft nofile 65536
elasticsearch                hard nofile 65536
elasticsearch                soft nproc 4096
elasticsearch                hard nproc 4096
elasticsearch                soft　　memlock　　unlimited
elasticsearch                hard　　memlock　　unlimited
################################################################
```


## 6. node
1. 执行`npm install`产生如下报错:
    ```
    npm ERR! code ELIFECYCLE
    npm ERR! errno 1
    npm ERR! phantomjs-prebuilt@2.1.16 install: `node install.js`
    npm ERR! Exit status 1
    npm ERR! 
    npm ERR! Failed at the phantomjs-prebuilt@2.1.16 install script.
    npm ERR! This is probably not a problem with npm. There is likely additional logging output above.
    npm ERR! A complete log of this run can be found in:
    npm ERR!     /root/.npm/_logs/2018-08-07T07_44_05_677Z-debug.log
    ```
    > 原因:不知道
    1. 解决方法:
    + 不用管,直接可以执行`npm run start`
2. 执行`npm install`产生如下报错:
    ```
    npm ERR! Linux 3.10.0-862.el7.x86_64
    npm ERR! argv "/usr/bin/node" "/usr/bin/npm" "install"
    npm ERR! node v6.14.3
    npm ERR! npm  v3.10.10
    npm ERR! code ELIFECYCLE

    npm ERR! phantomjs-prebuilt@2.1.16 install: `node install.js`
    npm ERR! Exit status 1
    npm ERR! 
    npm ERR! Failed at the phantomjs-prebuilt@2.1.16 install script 'node install.js'.
    npm ERR! Make sure you have the latest version of node.js and npm installed.
    npm ERR! If you do, this is most configure: error: libnfnetlink headers missinglikely a problem with the phantomjs-prebuilt package,
    npm ERR! not with npm itself.
    npm ERR! Tell the author that this fails on your system:
    npm ERR!     node install.js
    npm ERR! You can get information on how to open an issue for this project with:
    npm ERR!     npm bugs phantomjs-prebuilt
    npm ERR! Or if that isn't available, you can get their info via:
    npm ERR!     npm owner ls phantomjs-prebuilt
    npm ERR! There is likely additional logging output above.

    npm ERR! Please include the following file with any support request:
    npm ERR!     /root/tools/elasticsearch-head/npm-debug.log

    ```
    > 原因: 没有安装最新版的npm
    1. 解决方法
    + 安装最新版的npm就好了

## 7. GateOne
1. `'module' object has no attribute 'MarkerEvaluation'`
    > 原因:distribute版本太老了,更新就好,参考[这个网站](https://github.com/kevin1024/vcrpy/issues/178)
    ```
    pip install --upgrade distribute --user
    ```
2. `"'AutoExpireDict' object has no attribute '_key_watcher'" in <bound method AutoExpireDict.__del__ of {}> ignored`
    > 使用了最新版的tornado,而这个支持比如说4.5.3的tornado
    ```
    pip install tornado==4.5.3
    ```
3. `[W 180808 12:26:53 iostream:1332] SSL Error on 12 ('192.168.56.1', 33256): [SSL: HTTP_REQUEST] http request (_ssl.c:579)`
    > 没有使用HTTPS协议
    使用HTTPS去访问就好了


## 8. iptables
1. `iptables v1.4.21: can't initialize iptables table NAT': Table does not exist (do you need to insmod?)Perhaps iptables or your kernel needs to be upgraded.`
    > 除了加载该加载的模块外,NAT不能大写
    ```
    modprobe ip_tables
    modprobe iptable_filter
    modprobe iptable_nat
    modprobe ip_conntrack
    modprobe ip_conntrack_ftp
    modprobe ip_nat_ftp
    modprobe ipt_state
    lsmod | egrep ^ip
    #-----------------------------------------------------------------------------------------
    iptable_filter         12810  0 
    iptable_nat            12875  0 
    ip_tables              27126  2 iptable_filter,iptable_nat
    #-----------------------------------------------------------------------------------------

    # 所有数据包从eth0路由处理后,如果源地址是在192.168.1.0/24这个网段,我就将数据包的源地址改成10.0.0.7
    # -t nat而不是NAT!!大小写敏感的
    iptables -t nat -A POSTROUTING -s 10.0.0.0/24 -o eth0 -j SNAT --to-source 192.168.1.100
    ```
## 9. LVS
1. 安装LVS时出现下面错误
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
解决
```
sudo yum install libnl* popt* -y
```
2. 在阿里云的远程连接里设置好LO的IP后,virutal server就ping不通real server了
    > 可能阿里云的服务器不支持把.只能在虚拟机做的样子


## 10. Keepalived

1. `!!! OpenSSL is not properly installed on your system. !!! !!! Can not include OpenSSL headers files.!!!`
    > 没有安装openssl
    ```
    yum install -y openssl*
    ```
2. `configure: error: libnfnetlink headers missing`
```
yum install -y libnfnetlink-devel
```


## 11. bind
1. `lnh.com.zone:10: ignoring out-of-zone data (NS)`
    > 可能是您设定了一个记录项超出了当前 ORIGIN 的范围﹐例如 localhost. 或 siyongc. (也就是错误使用 FQDN )﹔或是错误的使用了 $ORIGIN 设定﹔或是在 named.conf 中有多个 zone 在分享同一份记录文件﹕ 


## 12. Apache
### 12-1. httpd
#### 12-1-1. mod_proxy
1. `No protocol handler was valid for the URL /demo (scheme 'balancer'). If you are using a DSO version of mod_proxy, make sure the proxy submodules are included in the configuration using LoadModule`
> 没有导入足够的proxy

```
LoadModule proxy_module modules/mod_proxy.so
LoadModule proxy_http_module modules/mod_proxy_http.so
LoadModule proxy_balancer_module modules/mod_proxy_balancer.so
```
## 13. VPN
### 13-1. OpenVPN
1. `configure: error: OpenSSL Crypto headers not found.`
```
yum install -y openssl-devel
```


## 14. GlusterFS
1. `staging failed on glusterfs 02. Error: /brick1/b1 is already part of a volume`
    1. 加上force就好了

## 15. Hadoop
1. 缺少ResourceManager和NodeManager
    1. 查看日志`hadoop-root-nodemanager-hadoop.log`,发现是没有找到Java类的意思(即问题2)
2. `java.lang.NoClassDefFoundError: javax/activation/DataSource`
    1. 原因:好像是JRE版本的问题,说使用Java8可以解决
    2. 解决
        1. 下载`jdk-8u181-linux-x64.tar.gz`
        2. 解压后移动到`/usr/local`下
        3. 将原来的`/usr/local/jdk`的软连接改成新的版本就好了
3. `ERROR org.apache.hadoop.hdfs.server.datanode.DataNode: Initialization failed for Block pool <registering> (Datanode Uuid 493518a8-fcee-4d3f-b5d0-9f66d9e76a3f) service to hadoop01/10.0.0.7:8020. Exiting.`
    1. 原因:克隆的时候原来的CLUSTER_ID保留了,而且格式化的时候只格式化nameNode,其他节点上没有进行,导致在`cat /tmp/hadoop-root/dfs/data/current/VERSION`(nameNode的话这里就是name而不是data了)里的CLUSTER_ID不一样.
    2. 解决
    ```
    ssh hadoop01 rm -rf /tmp/hadoop-root/*
    ssh hadoop02 rm -rf /tmp/hadoop-root/*
    ssh hadoop03 rm -rf /tmp/hadoop-root/*
    ssh hadoop04 rm -rf /tmp/hadoop-root/*

    ssh hadoop01 /usr/local/hadoop/bin/hdfs namenode -format 
    ssh hadoop02 /usr/local/hadoop/bin/hdfs namenode -format 
    ssh hadoop03 /usr/local/hadoop/bin/hdfs namenode -format 
    ssh hadoop04 /usr/local/hadoop/bin/hdfs namenode -format 
    ```
    
4. `localhost: ERROR: JAVA_HOME is not set and could not be found.`
    1. 解决
    ```
    vim /usr/local/hadoop/etc/hadoop-pseudo/hadoop-env.sh
    ##############################################################################
    export JAVA_HOME=/usr/local/jdk
    ##############################################################################
    ```
5. 完全分布式里,只有namenode进程没有启动?
    1. 原因:HDFS默认使用的是/tmp目录下,而/tmp重新开机就会被清空,等等原因所以会出现问题
    2. 解决:换个存储地方就好了
    ```
    vim /usr/local/hadoop/etc/hadoop-fully/core-site.xml # 添加下面的内容就好了,这里的话是创建在/hdfs目录下
    ##############################################################################
    <property>
         <name>hadoop.tmp.dir</name>
         <value>/hdfs</value>
         <description>A base for other temporary directories.</description>
    </property>
    ##############################################################################

    for i in {hadoop02,hadoop03};do scp /usr/local/hadoop/etc/hadoop-fully/core-site.xml root@"$i":/usr/local/hadoop/etc/hadoop-fully/core-site.xml; done
    
    for i in {hadoop01,hadoop02,hadoop03};do ssh "$i" mkdir /hdfs;done
    for i in {hadoop01,hadoop02,hadoop03};do ssh "$i" /usr/local/hadoop/bin/hdfs namenode -format;done
    
    start-all.sh 
    jps
    #------------------------------------------------------------------------------------------
    # 61570 NameNode
    # 62706 Jps
    # 62099 ResourceManager
    # 61847 SecondaryNameNode
    #------------------------------------------------------------------------------------------

    ```
