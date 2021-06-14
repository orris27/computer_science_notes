    
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

### 3-3. vmware
1. 虚拟机ping不通`192.168.56.1`和`192.168.56.2`
    1. 金山毒霸这些软件有开机加速,如果他们屏蔽了VMware Workstation的话,那么就会导致这个问题.所以解决方法就是允许VMware Workstation开机启动
2. 无法使用xshell连接虚拟机里的Ubuntu
    1. 原因: 没有启动ssh
    2. 解决: `sudo apt-get install ssh`

3. `make` reports that PIC mode
    1. solution: `KBUILD_CFLAGS += -fno-pic`

### 3-4. kernel
1. 编译内核时`make`报错:
```
/boot/config-4.15.0-39-generic:4243:warning: symbol value 'm' invalid for GPIO_MB86S7X
Your display is too small to run Menuconfig!
It must be at least 19 lines by 80 columns.
scripts/kconfig/Makefile:28: recipe for target 'menuconfig' failed
make[1]: *** [menuconfig] Error 1
Makefile:547: recipe for target 'menuconfig' failed
make: *** [menuconfig] Error 2
```
解决: 因为它说display is too small,所以只要`Ctrl + -`缩小terminal就好了

2. 
```
./include/linux/linkage.h:8:10: fatal error: asm/linkage.h: No such file or directory
 #include <asm/linkage.h>
          ^~~~~~~~~~~~~~~
compilation terminated.
```

3. 
```
make -C /lib/modules/4.15.0-29-generic/build M=/home/orris/linux-3.18.24/fs/myext2  modules
make[1]: Entering directory '/usr/src/linux-headers-4.15.0-29-generic'
  CC [M]  /home/orris/linux-3.18.24/fs/myext2/balloc.o
/home/orris/linux-3.18.24/fs/myext2/balloc.c: In function ‘myext2_has_free_blocks’:
/home/orris/linux-3.18.24/fs/myext2/balloc.c:1188:26: error: implicit declaration of function ‘current_fsuid’; did you mean ‘current_umask’? [-Werror=implicit-function-declaration]
   !uid_eq(sbi->s_resuid, current_fsuid()) &&
                          ^~~~~~~~~~~~~
                          current_umask
/home/orris/linux-3.18.24/fs/myext2/balloc.c:1188:26: error: incompatible type for argument 2 of ‘uid_eq’
In file included from ./include/linux/stat.h:20:0,
                 from ./include/linux/fs.h:10,
                 from /home/orris/linux-3.18.24/fs/myext2/myext2.h:13,
                 from /home/orris/linux-3.18.24/fs/myext2/balloc.c:14:
./include/linux/uidgid.h:61:20: note: expected ‘kuid_t {aka struct <anonymous>}’ but argument is of type ‘int’
 static inline bool uid_eq(kuid_t left, kuid_t right)
                    ^~~~~~
/home/orris/linux-3.18.24/fs/myext2/balloc.c:1190:5: error: implicit declaration of function ‘in_group_p’ [-Werror=implicit-function-declaration]
    !in_group_p (sbi->s_resgid))) {
     ^~~~~~~~~~
cc1: some warnings being treated as errors
scripts/Makefile.build:332: recipe for target '/home/orris/linux-3.18.24/fs/myext2/balloc.o' failed
make[2]: *** [/home/orris/linux-3.18.24/fs/myext2/balloc.o] Error 1
Makefile:1552: recipe for target '_module_/home/orris/linux-3.18.24/fs/myext2' failed
make[1]: *** [_module_/home/orris/linux-3.18.24/fs/myext2] Error 2
make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-29-generic'
Makefile:11: recipe for target 'default' failed
make: *** [default] Error 2
```


4. `sudo bash mkfs.myext2 myfs` reports:
```
losetup: myfs: failed to set up loop device: Device or resource busy
mke2fs 1.44.1 (24-Mar-2018)
/dev/loop12 contains a squashfs file system
Proceed anyway? (y,N) y
/dev/loop12 is mounted; will not make a filesystem here!
2+0 records in
2+0 records out
2048 bytes (2.0 kB, 2.0 KiB) copied, 0.000129319 s, 15.8 MB/s
previous magic number is 0x4797
current magic number is 0x6666
change magic number ok!
dd: writing to '/dev/loop12': Operation not permitted
1+0 records in
0+0 records out
0 bytes copied, 0.00717034 s, 0.0 kB/s

```

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
3. 执行`npm install`报错:
    ```
    PhantomJS not found on PATH
    Download already available at /tmp/phantomjs/phantomjs-2.1.1-linux-x86_64.tar.bz2
    Verified checksum of previously downloaded file
    Extracting tar contents (via spawned process)
    Removing /root/template/bilibili-vue/node_modules/phantomjs-prebuilt/lib/phantom
    Copying extracted folder /tmp/phantomjs/phantomjs-2.1.1-linux-x86_64.tar.bz2-extract-1543990017617/phantomjs-2.1.1-linux-x86_64 -> /root/template/bilibili-vue/node_modules/phantomjs-prebuilt/lib/phantom
    Phantom installation failed { [Error: EACCES: permission denied, link '/tmp/phantomjs/phantomjs-2.1.1-linux-x86_64.tar.bz2-extract-1543990017617/phantomjs-2.1.1-linux-x86_64' -> '/root/template/bilibili-vue/node_modules/phantomjs-prebuilt/lib/phantom']
      errno: -13,
      code: 'EACCES',
      syscall: 'link',
      path:
       '/tmp/phantomjs/phantomjs-2.1.1-linux-x86_64.tar.bz2-extract-1543990017617/phantomjs-2.1.1-linux-x86_64',
      dest:
       '/root/template/bilibili-vue/node_modules/phantomjs-prebuilt/lib/phantom' } Error: EACCES: permission denied, link '/tmp/phantomjs/phantomjs-2.1.1-linux-x86_64.tar.bz2-extract-1543990017617/phantomjs-2.1.1-linux-x86_64' -> '/root/template/bilibili-vue/node_modules/phantomjs-prebuilt/lib/phantom'
    Segmentation fault
    ```
    > 原因: 没有权限?不过这里的权限好像不是指root的
    1. 解决方法
    ```
    rm -rf ./node_modules/
    npm install --unsafe-perm
    ```
4. `npm install` reports:
    ```
    npm ERR! code ELIFECYCLE
    npm ERR! errno 1
    npm ERR! chromedriver@2.44.1 install: `node install.js`
    npm ERR! Exit status 1
    npm ERR! 
    npm ERR! Failed at the chromedriver@2.44.1 install script.
    npm ERR! This is probably not a problem with npm. There is likely additional logging output above.

    npm ERR! A complete log of this run can be found in:
    npm ERR!     /root/.npm/_logs/2018-12-05T06_28_45_887Z-debug.log
    ```
    1. solution: 删除原先安装的,然后执行下面代码,最后`npm install`. 参考网站:[ref](https://stackoverflow.com/questions/38323880/error-eacces-permission-denied)
    ```
    # 下面的代码具体哪个起到了作用我不是很清楚
    mkdir ~/.npm-global
    npm config set prefix '~/.npm-global'
    export PATH=~/.npm-global/bin:$PATH
    NPM_CONFIG_PREFIX=~/.npm-global
    rm -rf node_modules/
    npm install chromedriver
    ```
5. `npm install` reports:然后等待很长时间
    ```
    Current existing ChromeDriver binary is unavailable, proceding with download and extraction.
    Downloading from file:  https://chromedriver.storage.googleapis.com/2.44/chromedriver_linux64.zip
    Saving to file: /tmp/chromedriver/chromedriver_linux64.zip
    ```
    运行的话报错:
    ```
     error  in ./src/App.vue?vue&type=style&index=0&lang=scss&

    Module build failed (from ./node_modules/sass-loader/lib/loader.js):
    Error: ENOENT: no such file or directory, scandir '/home/orris/fun/node/CoreUI-Vue/node_modules/node-sass/vendor'
        at Object.fs.readdirSync (fs.js:861:3)
        at Object.1getInstalledBinaries (/home/orris/fun/node/CoreUI-Vue/node_modules/node-sass/lib/extensions.js:130:13)
        at foundBinariesList (/home/orris/fun/node/CoreUI-Vue/node_modules/node-sass/lib/errors.js:20:15)
        at foundBinaries (/home/orris/fun/node/CoreUI-Vue/node_modules/node-sass/lib/errors.js:15:5)
        at Object.module.exports.missingBinary (/home/orris/fun/node/CoreUI-Vue/node_modules/node-sass/lib/errors.js:45:5)
        at module.exports (/home/orris/fun/node/CoreUI-Vue/node_modules/node-sass/lib/binding.js:15:30)
        at Object.<anonymous> (/home/orris/fun/node/CoreUI-Vue/node_modules/node-sass/lib/index.js:14:35)
        at Module._compile (internal/modules/cjs/loader.js:678:30)
        at Object.Module._extensions..js (internal/modules/cjs/loader.js:689:10)
        at Module.load (internal/modules/cjs/loader.js:589:32)
        at tryModuleLoad (internal/modules/cjs/loader.js:528:12)
        at Function.Module._load (internal/modules/cjs/loader.js:520:3)
        at Module.require (internal/modules/cjs/loader.js:626:17)
        at require (internal/modules/cjs/helpers.js:20:18)
        at Object.sassLoader (/home/orris/fun/node/CoreUI-Vue/node_modules/sass-loader/lib/loader.js:46:72)
    ```
    1. 解决:先安装chromedriver
    ```
    npm install chromedriver
    npm install
    ```
6. `ENOSPC: no space left on device, watch `,但是`df -h`和`df -i`都没有满
    1. 解决:
    ```
    cat /proc/sys/fs/inotify/max_user_watches # 查看watches数量
    sudo sysctl fs.inotify.max_user_watches=16384 # 暂时修改
    #echo 16384 | sudo tee -a /proc/sys/fs/inotify/max_user_watches # 永久修改
    ```


7. `npm run dev` reports:
    ```
    ERROR in ./~/css-loader!./~/vue-loader/lib/style-compiler?{"id":"data-v-0a3f9cc3","scoped":false,"hasInlineConfig":true}!./~/sass-loader/lib/loader.js!./~/vue-loader/lib/selector.js?type=styles&index=0!./~/vue-bulma-tabs/src/Tabs.vue
    Module build failed: 
    @import '~bulma/sass/utilities/initial-variables';
    ^
          File to import not found or unreadable: ~bulma/sass/utilities/initial-variables.
          in /home/orris/fun/node/vue-admin/node_modules/vue-bulma-tabs/src/Tabs.vue (line 114, column 1)
     @ ./~/vue-style-loader!./~/css-loader!./~/vue-loader/lib/style-compiler?{"id":"data-v-0a3f9cc3","scoped":false,"hasInlineConfig":true}!./~/sass-loader/lib/loader.js!./~/vue-loader/lib/selector.js?type=styles&index=0!./~/vue-bulma-tabs/src/Tabs.vue 4:14-264 13:3-17:5 14:22-272
     @ ./~/vue-bulma-tabs/src/Tabs.vue
     @ ./~/vue-bulma-tabs/src/index.js
     @ ./~/babel-loader/lib!./~/vue-loader/lib/selector.js?type=script&index=0!./client/views/components/Tabs.vue
     @ ./client/views/components/Tabs.vue
     @ ./client/views lazy ^\.\/.*\.vue$
     @ ./client/store/modules/menu/lazyLoading.js
     @ ./client/store/modules/menu/index.js
     @ ./client/store/index.js
     @ ./client/app.js
     @ ./client/index.js
     @ multi ./build/dev-client ./client/index.js

    ```
    1. 解决(失败): `npm install stylus-loader css-loader style-loader --save-dev`
    2. 解决: 参考issues里面的编译错误问题[issues](https://github.com/vue-bulma/vue-admin/issues)
    ```
    cd node_modules/bulma/sass/utilities
    wget https://raw.githubusercontent.com/jgthms/bulma/master/sass/utilities/initial-variables.sass
    cd -
    npm run dev
    ```
8. 
```
To install them, you can run: npm install --save @/api/article @/components/Pagination @/directive/waves @/vendor/Export2Excel xlsx script-loader!file-saver
```
solution: 如果是这种`@`开头的表示没有这个文件,只要创建这些文件就可以了,但是xlsx肯定是要`npm install --save xlsx`.!表示两个都要(好像)
```
npm install file-saver --save
npm install xlsx --save
npm install script-loader --save-dev
```


9. 
```
Module Error (from ./node_modules/vue-loader/lib/loaders/templateLoader.js):
(Emitted value instead of an instance of Error) 
  Error compiling template:
  
  <div class="container">
  
    <el-tabs v-model="activeName" @tab-click="handleClick">
      <el-tab-pane label="All" name="all">
        <Table>
      </el-tab-pane>
      <el-tab-pane label="New" name="new">No data</el-tab-pane>
      <el-tab-pane label="Repairing" name="repairing">No data</el-tab-pane>
      <el-tab-pane label="to be commented" name="comment">No data</el-tab-pane>
    </el-tabs>
  </div>
  
  - tag <Table> has no matching end tag.


 @ ./src/views/repair/myrepair/index.vue?vue&type=template&id=240e2d94& 1:0-215 1:0-215
 @ ./src/views/repair/myrepair/index.vue
 @ ./src/router/index.js
 @ ./src/main.js
 @ multi (webpack)-dev-server/client?http://localhost:9528 (webpack)/hot/dev-server.js ./src/main.js

```
solution: use `<Table></Table>` (add `</Table>`)


10. `npm run dev`还是跑在localhost里面
+ `config/index.js`中修改localhost为`0.0.0.0`
+ 或者在`node_modules/webpack-dev-server/bin/webpack-dev-server.js`中修改localhost为`0.0.0.0`


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
6. 使用完全分布式执行JobMain这个jar程序,出现错误`Could not find or load main class org.apache.hadoop.mapreduce.v2.app.MRAppMaster`
    1. 解决
    ```
    vim /usr/local/hadoop/etc/hadoop-fully/mapred-site.xml # 添加下面内容就行了.里面的value是hadoop classpath里面的内容
    #################################################################################################
    <property> 
        <name>mapreduce.application.classpath</name>
        <value>/usr/local/hadoop-3.1.1/etc/hadoop:/usr/local/hadoop-3.1.1/share/hadoop/common/lib/*:/usr/local/hadoop-3.1.1/share/hadoop/common/*:/usr/local/hadoop-3.1.1/share/hadoop/hdfs:/usr/local/hadoop-3.1.1/share/hadoop/hdfs/lib/*:/usr/local/hadoop-3.1.1/share/hadoop/hdfs/*:/usr/local/hadoop-3.1.1/share/hadoop/mapreduce/lib/*:/usr/local/hadoop-3.1.1/share/hadoop/mapreduce/*:/usr/local/hadoop-3.1.1/share/hadoop/yarn:/usr/local/hadoop-3.1.1/share/hadoop/yarn/lib/*:/usr/local/hadoop-3.1.1/share/hadoop/yarn/*</value>
    </property>
    #################################################################################################

    #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    # 推送配置文件到其他节点,删除原来的目录,格式化,启动Hadoop,执行
    #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ```
## 16. UNIX网络编程
### 16-1. POSIX
1. `mq_send: Bad file descriptor`
    1. 解决:要以能写入的方式打开POSIX消息队列
2. `mq_send: Message too long`
    1. 解决:接收消息时的消息长度必须是单个消息的最大字节长度
    ```
    struct mq_attr attr;
    if((mq_getattr(mqid,&attr)) == -1)
        ERR_EXIT("mq_getattr");
    //attr.mq_msgsize就是单个消息的最大字节长度
    ```
3. `Segmentation fault (core dumped)`
    1. 解决:打开文件和映射文件的权限要对应.比如只读打开文件(O_RDONLY)就不能在映射的时候用write(mmap中不能用PROT_WRITE)
4. `undefined reference to 'pthread_create'`
    1. 解决:`man pthread_create`,知道要在连接的时候添加`-pthread`参数


## 17. UNIX编程
1. gdb中出现`No symbol table is loaded.  Use the "file" command.`
    1. 解决:编译时加`-g`参数
2. mmap出现`Bus error (core dumped)`
    1. 解决:将实体文件的大小改到能够容纳所有数据.比如临时文件的话,必须ftruncate其大小才行
3. munmap出现`munmap: Invalid argument`
    1. 解决:移动mmap返回的p指针回到起始位置
4. No manual entry for pthread_create. `man pthread_create`.
```
yum install -y man-pages

sudo apt-get install manpages-posix-dev

##############################################
user@ubuntu:~$ sudo apt-get install manpages # tab
manpages            manpages-es-extra   manpages-it         manpages-posix      manpages-zh
manpages-de         manpages-fr         manpages-ja         manpages-posix-dev  
manpages-de-dev     manpages-fr-dev     manpages-ja-dev     manpages-pt         
manpages-dev        manpages-fr-extra   manpages-pl         manpages-pt-dev     
manpages-es         manpages-hu         manpages-pl-dev     manpages-tr    
```
## 18. yum
1. glibc的问题
```
Error: Package: glibc-common-2.17-196.el7_4.2.x86_64 (@updates)
           Requires: glibc = 2.17-196.el7_4.2
           Removing: glibc-2.17-196.el7_4.2.x86_64 (@updates)
               glibc = 2.17-196.el7_4.2
           Updated By: glibc-2.17-222.el7.x86_64 (base)
               glibc = 2.17-222.el7
 You could try using --skip-broken to work around the problem
** Found 3 pre-existing rpmdb problem(s), 'yum check' output follows:
glibc-common-2.17-222.el7.x86_64 is a duplicate with glibc-common-2.17-196.el7_4.2.x86_64
glibc-common-2.17-222.el7.x86_64 has missing requires of glibc = ('0', '2.17', '222.el7')
libgcc-4.8.5-28.el7_5.1.x86_64 is a duplicate with libgcc-4.8.5-16.el7_4.2.x86_64
```
解决: `rpm -evh glibc-common-2.17-222.el7.x86_64`
## 19. TensorFlow
1. `unhashable type: 'numpy.ndarray'`
    + 注意:
        1. numpy array是可以传入到tf.float32这些中去的
        2. numpy array中如果是int类型,也可以传入到tf.float32中
    + 问题所在:placeholder的名字和传入的变量的名字重复了.比如我`feed_dict = {labels:labels}`就是错误的
    
2. `ValueError: Dimensions must be equal, but are 60 and 40 for 'rnn/while/rnn/multi_rnn_cell/cell_0/basic_lstm_cell/MatMul_1' (op: 'MatMul') with input shapes: [?,60], [40,120].`
    + 原因:使用MultiRNNCell的时候,重复利用了一层循环体.
    + 解决
    ```
    def create_lstm_cell(lstm_size, output_keep_prob):
        lstm_cell=tf.contrib.rnn.BasicLSTMCell(lstm_size,state_is_tuple=True)
        lstm_cell = tf.nn.rnn_cell.DropoutWrapper(lstm_cell, output_keep_prob=output_keep_prob)
        return lstm_cell

    stacked_lstm = tf.nn.rnn_cell.MultiRNNCell([create_lstm_cell(FLAGS.lstm_size, FLAGS.keep_prob) for _ in range(num_layers)])
    ```
3. `module 'tensorflow' has no attribute 'FIFOQueue'`
    + 文件名和模块冲突了.比如`queue.py`就冲突了
    
4. `ValueError: Variable rnn/multi_rnn_cell/cell_0/basic_lstm_cell/kernel already exists, disallowed. Did you mean to set reuse=True or reuse=tf.AUTO_REUSE in VarScope? Originally defined at:`
    1. 原因: 可能是连续对不同lstm_cell调用2次tf.nn.dynamic_rnn
    ```
    _, encoder_state = tf.nn.dynamic_rnn(self.enc_lstm_cell,src_embedded_chars,dtype=tf.float32)
    outputs,final_state = tf.nn.dynamic_rnn(self.dec_lstm_cell,trg_embedded_chars,dtype=tf.float32,initial_state=encoder_state)
    ```
    2. 解决: 添加scope就好了
    ```
    with tf.variable_scope('encode'):
        _, encoder_state = tf.nn.dynamic_rnn(self.enc_lstm_cell,src_embedded_chars,dtype=tf.float32)
    with tf.variable_scope('decode'):
        outputs,final_state = tf.nn.dynamic_rnn(self.dec_lstm_cell,trg_embedded_chars,dtype=tf.float32,initial_state=encoder_state)

    ```
    
5. `FailedPreconditionError (see above for traceback): Attempting to use uninitialized value decode/rnn/multi_rnn_cell/cell_1/basic_lstm_cell/kernel`
    1. 原因: 部分tensor变量没有初始化
    2. 解决: 我在设计Seq2Seq的时候,根据教材写了model.forward这个函数(里面包括embedding_lookup, dynamic_rnn, train等),但是我把这个forward函数的调用写在了session下的epoch循环体里,就导致我原来一开始初始化全局变量的时候,没有初始化这些内容.所以我把forward的调用提到了session前面,就OK了
    
    
6. `tensorflow.python.framework.errors_impl.FailedPreconditionError: Attempting to use uninitialized value cell_0/basic_lstm_cell/bias`
    1. 原因: 我使用`class Model`的方式定义的,但是在创建对象的时候,原来的`train.py`中放在"model"的scope下,但是在`eval.py`中没有把`model = Model(...)`放在"model"的scope下,所以就产生了这个错误
    2. 解决: 把eval.py的定义写到scope下就行了



7. `TypeError: Cannot interpret feed_dict key as Tensor: Tensor Tensor("placeholder/features:0", shape=(8,), dtype=int32) is not an element of this graph.`
    1. 原因: 定义的tensor不在默认的graph里面
    2. 解决
    ```
    graph = tf.Graph()
    with graph.as_default():
        pass
    # ...
    with tf.Session(graph=graph) as sess:
        pass
    ```

8. `TypeError: Fetch argument 201.4594 has invalid type <class 'numpy.float32'>, must be a string or Tensor. (Can not convert a float32 into a Tensor or Operation.)`
    1. 原因: 我在图中定义了tensor的loss变量,而在sess.run的时候用loss变量去接收了.也就是说我定义了两个相同名字的变量.
    2. 解决: 换个名字就好了
    ```
    # 取名字为loss1
    _, loss1 = sess.run([train, loss], feed_dict = {features:X, labels:y})
    ```
    
9. 使用`sess.run(grads)`时报错`TypeError: Fetch argument None has invalid type <class 'NoneType'>`
    1. 原因: 我的grads使用`tf.gradients(loss, params)`,而params是通过`tf.trainable_variables()`获得的,但是在模型中我计算loss时可能没用到所有可训练的变量,导致有些可训练变量计算出来的梯度为None,也就是没有计算梯度.理论上所有可训练变量在计算loss时都会被使用到,但是部分没有被运行很可能是因为我变量写错了
    2. 解决
    ```
    # 6th layer: nn: [batch_size, 7 * 7 * 512] => [batch_size, 4096]
    nn6 = self._nn(reshape1, 4096, tf.tanh, 'layer6')
    nn6_dropout = tf.nn.dropout(nn6,keep_prob)

    # 7th layer: nn: [batch_size, 4096] => [batch_size, 4096]
    nn7 = self._nn(nn6, 4096, tf.tanh, 'layer7') # 比如这里的写错成nn6就会导致错误,应该是nn6_dropout
    nn7_dropout = tf.nn.dropout(nn7,keep_prob)

    ```
    
10. 使用Slim做LeNet-5模型来训练MNIST数据集时参数没有更新:
    ```
    epoch 0: 1028/10000
    epoch 50: 1032/10000
    epoch 100: 1032/10000
    epoch 150: 1032/10000
    epoch 200: 1032/10000
    epoch 250: 1032/10000
    epoch 300: 1032/10000
    epoch 350: 1032/10000
    ```
    1. 原因: loss的计算公式不对:`loss=-tf.reduce_mean(tf.reduce_sum(labels*tf.log(y_predicted)))`这个是错误的
    2. 解决: loss的计算公式更正为: `loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(logits=y_predicted, labels=tf.argmax(labels, 1)))`
    
    
11. `ValueError: 'size' must be a 1-D Tensor of 2 elements: new_height, new_width`
    1. 原因: `image = tf.image.resize_images(image,image_h,image_w)`
    2. 解决: `image = tf.image.resize_images(image,(image_h,image_w))`


12. `ValueError: Can't convert Python sequence with mixed types to Tensor.`
    1. 原因: `dataset = tf.data.Dataset.from_tensor_slices([filenames_train,padded_indices_train])`
    2. 解决: `dataset = tf.data.Dataset.from_tensor_slices((filenames_train,padded_indices_train))`


13. 使用tf.keras里的Dense的时候,报错`ValueError: Input 0 of layer dense is incompatible with the layer: : expected min_ndim=2, found ndim=1. Full shape received: [64]`
    1. 原因: 全连接神经网络的输入不能只有1个维度,也就是说我传入的是`[64]`形状的,所以至少要2个维度
    2. 解决: 变成`[64, x]`或者`[x, 64]`


14. 跑TF官网的image caption时报错`tensorflow.python.framework.errors_impl.UnknownError: Fail to find the dnn implementation. [Op:CudnnRNN]`
    1. 原因: 使用eager模式时config设置了0.7的使用率
    2. 解决: 取消这个设置就好了



15. 执行`python xx.py`时报错`SyntaxError: unexpected EOF while parsing`
    1. 原因
    + 最后的语句少了一个括号, 比如`print("step {0}: loss={1}".format(step, loss / padded_indices.shape[1])`
    + 有个indent的地方没有是使用`pass`或者写代码


16. 执行`loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = labels,logits = logits) * mask)`时报错
    ```
    Node: {{node SparseSoftmaxCrossEntropyWithLogits}} = SparseSoftmaxCrossEntropyWithLogits[T=DT_INT32, Tlabels=DT_INT32](dummy_input, dummy_input)
    All kernels registered for op SparseSoftmaxCrossEntropyWithLogits :
      device='CPU'; T in [DT_HALF]; Tlabels in [DT_INT64]
      device='CPU'; T in [DT_HALF]; Tlabels in [DT_INT32]
      device='CPU'; T in [DT_DOUBLE]; Tlabels in [DT_INT64]
      device='CPU'; T in [DT_DOUBLE]; Tlabels in [DT_INT32]
      device='CPU'; T in [DT_FLOAT]; Tlabels in [DT_INT64]
      device='CPU'; T in [DT_FLOAT]; Tlabels in [DT_INT32]
      device='GPU'; T in [DT_HALF]; Tlabels in [DT_INT64]
      device='GPU'; T in [DT_HALF]; Tlabels in [DT_INT32]
      device='GPU'; T in [DT_FLOAT]; Tlabels in [DT_INT64]
      device='GPU'; T in [DT_FLOAT]; Tlabels in [DT_INT32]
     [Op:SparseSoftmaxCrossEntropyWithLogits]
    ```
    1. 原因: 对于`sparse_softmax_cross_entropy_with_logits`, logits必须是`[batch_size, num_classes]`,而labels必须是`[batch_size]`.报这个错多半是logits或者labels的shape不对


17. 执行`loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = labels,logits = logits) * mask)`时报错
    ```
    Traceback (most recent call last):
      File "my.py", line 345, in <module>
        loss += calc_loss(logits=output, labels=padded_indices[:, curr_timestep])
      File "my.py", line 295, in calc_loss
        loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = labels,logits = logits) * mask)
      File "/home/user/anaconda2/envs/ovenv/lib/python3.6/site-packages/tensorflow/python/ops/nn_ops.py", line 2063, in sparse_softmax_cross_entropy_with_logits
        (labels_static_shape.ndims, logits.get_shape().ndims))
    ValueError: Rank mismatch: Rank of labels (received 1) should equal rank of logits minus 1 (received 3).
    ```
    1. 原因: 见报错,logits的dims应该是2




18. loss的问题
    1. 一直没有减少,或者减少的几率很少
        1. 学习率是不是设置错误.比如0.01设置成0.1之类的
        2. optimizer必须不同: eager mode中:optimizer的定义`optimizer = tf.train.AdamOptimizer()`是不是设置在内部的epoch循环内.(参考我模仿TF官网的image-caption)
        3. GradientTape必须不同: GAN中: Generator和Discriminator的GradientTape和optimizer都要不同才行
    2. 学习TensorFlow官网的DCGAN代码自己写后,发现Generator的loss不断上升
        ```
        epoch=0    gen_loss=2.8731160163879395    disc_loss=0.05911536514759064
        epoch=0    gen_loss=2.9348721504211426    disc_loss=0.059374239295721054
        epoch=0    gen_loss=2.995558023452759    disc_loss=0.059433963149785995
        epoch=0    gen_loss=3.054912805557251    disc_loss=0.059680353850126266
        epoch=0    gen_loss=3.112762212753296    disc_loss=0.04616463556885719
        ```
        1. 我在Generator中使用了BatchNormal,而在Discriminator中使用了Dropout,这些在训练和不训练的时候表现不同,所以我要定义training属性.而我之前没有定义training=False这些,这可能是导致loss增加的原因之一


19. 使用了`python xx.py 2>&1 >xx.out`和`print`函数,但为什么有的会直接输出到文件里,有的要等到程序结束后才输出过去?
    1. 原因: 具体差别我不知道,总之不直接输出到文件的可以直接使用`sys.stdout.flush()`




20. restore图的时候报错
    ```
    Assign requires shapes of both tensors to match. lhs shape= [2,1,512] rhs shape= [2,64,512]
         [[{{node save/Assign_5}} = Assign[T=DT_FLOAT, _class=["loc:@Generator/decoder_1/Variable"], use_locking=true, validate_shape=true, _device="/job:localhost/replica:0/task:0/device:GPU:0"](Generator/decoder_1/Variable, save/RestoreV2/_11)]]
    ```
    1. 原因：名字为`Generator/decoder_1/Variable`(`:0`)的形状和存储中的形状不一样，存储的是`[2,64,512]`,而restore时是`[2, 1 512]`,所以只要改变该变量的shape就可以了.当然,查找到该变量只要用`tf.get_tensor_by_name`就可以了,配合`tf.global_variables()`等.
    2. 解决: 实际上是`self.initial_state = tf.Variable(self.lstm_cell.zero_state(self.batch_size, tf.float32), trainable=False)`造成,但是这个变量实际上在后续是没有多大用的,但是又需要`initial_state = sess.run(self.generator.inference_initial_state, {...})`这里来运行,所以不能直接删除`self.initial_state`这个变量.所以最终只要不要把这个`self.initial_state`放到saver里面save就可以了.所以只要去掉`tf.Variable`就行,即`self.initial_state = self.lstm_cell.zero_state(self.batch_size, tf.float32)`
    + 注意: 我一开始的解决方法为了restore成功,就把`train.py`里的`batch_size`改成1,后期训练很慢,而且成功率很低,所以最后就不行了


21. GAN来做手写数字生成的时候报错
    ```
    step 0: g_loss=nan    d_loss=inf
    step 1000: g_loss=nan    d_loss=nan
    step 2000: g_loss=nan    d_loss=nan
    ```
    1. 原因:
    + 我的D和G都是2层神经网络,激活函数都是sigmoid+softmax.结果换成relu+sigmoid就解决这个问题了
    + 初始化W和b也有问题
    + ...

22. dcgan-mnist.py里报错
    ```
    Traceback (most recent call last):
      File "dcgan-mnist.py", line 144, in <module>
        real_pred = discriminator(images)
      File "/home/user/anaconda2/envs/ovenv/lib/python3.6/site-packages/tensorflow/python/keras/engine/base_layer.py", line 769, in __call__
        outputs = self.call(inputs, *args, **kwargs)
      File "dcgan-mnist.py", line 90, in call
        net = self.conv1(inputs)
      File "/home/user/anaconda2/envs/ovenv/lib/python3.6/site-packages/tensorflow/python/keras/engine/base_layer.py", line 743, in __call__
        self._assert_input_compatibility(inputs)
      File "/home/user/anaconda2/envs/ovenv/lib/python3.6/site-packages/tensorflow/python/keras/engine/base_layer.py", line 1474, in _assert_input_compatibility
        if x.shape.ndims is None:
    AttributeError: 'tuple' object has no attribute 'ndims'

    ```
    1. 解决: 之前用的mnist是`mnist = input_data.read_data_sets('MNIST_data/',one_hot = True)`,实际应该使用的是`(X_train, y_train), (X_test, y_test) = mnist.load_data()`


23. Op type not registered 'NonMaxSuppressionV3' in binary running on orris-Laptop.
    1. 解决: 严格修改tensorflow的版本为该代码指定的版本

24. `import tensorflow` reports:
```
ImportError: libcublas.so.9.0: cannot open shared object file: No such file or directory
```
安装9.0版本的cuda就可以了,比如`sudo apt-get install cuda-9-0`


25. `Unknown command line flag 'f'`

原因: 我也不知道,jupyter notebook里会报错,但是命令行不会

解决: 添加这一行就可以了
```
tf.app.flags.DEFINE_string('f', '', 'kernel')
```



## 20. gym
1. 执行`env.render()`报错`ImportError: sys.meta_path is None, Python is likely shutting down`
    1. 解决: 添加`env.close()`


## 21. blacklist.conf
1. `libkmod: ERROR ../libkmod/libkmod-config.c:656 kmod_config_parse: /etc/modprobe.d/blacklist-nouveau.conf line 1: ignoring bad line starting with 'cklist'`
    1. 查看blacklist-nouveau.conf文件:
    ```
    cklist nouveau # 这里的cklist是不正确的,可以看到应该是blacklist,所以改成blacklist就可以了
    blacklist lbm-nouveau
    options nouveau modeset=0
    alias nouveau off
    alias lbm-nouveau off
    ```

## 22. Ubuntu
1. `sudo apt-get update`的时候报错`E: The repository 'http://ppa.launchpad.net/fcitx-team/nightly/ubuntu artful Release' does not have a Release file.N: Updating from such a repository can't be done securely, and is therefore disabled by default.`
    1. 解决: 直接删除对应的文件就行了
    ```
    cd /etc/apt/sources.list.d/
    ls
    sudo mv fcitx-team-ubuntu-nightly-artful.list fcitx-team-ubuntu-nightly-artful.list.bak
    ```
2. `sudo apt-get update`连接不上`Connecting to storage.googleapis.com`
    + 解决
    ```
    diff t-tujikawa-ubuntu-ppa-artful.list t-tujikawa-ubuntu-ppa-artful.list.save # 如果没有输出,就说明已经有备份了,直接删除
    
    sudo rm t-tujikawa-ubuntu-ppa-artful.list
    ```
3. 执行`unity-tweak-tool`时报错`Error: schema com.canonical.notify-osd not installed`
    + 解决: `sudo apt-get install notify-osd`


4. 开机启动无法进入桌面,显示`before the ppp link was shut down`:
    + 解决1: 从gdm3切换到lightdm
    ```
    sudo apt-get install lightdm
    sudo dpkg-reconfigure lightdm # 这里的lightdm是指切换到lightdm,可以换成gdm3
    reboot
    ```
    + solution2: reinstall gdm3
    ```
    sudo apt purge gdm gdm3
    sudo apt install gdm3 ubuntu-desktop
    sudo dkpg-reconfigure gdm3
    reboot
    ```
    + (final)无法进去的原因:通过`systemctl status gdm3`发现里面有一行说查看xserver的log,然后检查`/var/log/Xorg.0.log`,发现EE表示error,搜索EE的行,里面有提示NVIDIA的driver版本不正确,所以就尝试重新安装NVIDIA的driver(当时的版本是396,最后安装的还是396,说明我之前安装存在问题,也就是说不能用apt安装,而是用runfile安装)
    ```
    Ctrl + Alt + F4
    sudo apt remove nvidia-396
    ps aux| grep gdm # 如果显示内容,说明gdm开启中,关闭gdm
    systemctl stop gdm
    sudo ./NVIDIA-Linux-x86_64-396.54.run -no-opengl-files
    reboot # 发现循环登录
    reboot # 直接再reboot就能解决循环登录的问题了
    ```


5. Ubunut登录成功后进入黑屏,只有鼠标

原因: 可能安装输入法出问题了

解决: 
    1. 黑屏的时候按Ctrl+Alt+F3(也可能是黑屏前,顺序我也忘了;此外也可以尝试其他F1,F2,F4等,直到进入命令行)
    2. 进入命令行
    3. top发现XOrg占用很大的CPU
    4. 卸载fcitx的所有内容
    ```
    sudo apt-get remove fcitx
    sudo apt-get remove fcitx-module*
    sudo apt-get remove fcitx-frontend*
    sudo apt-get purge fcitx*
    ```
    5. 卸载sogou拼音
    ```
    sudo dpkg -l so* # 可以看到sogoupinyin
    sudo apt-get purge sogoupinyin
    ```
    6. 重启
    7. 能进入桌面了

6. mount USB fail
```
$MFTMirr does not match $MFT (record 0).
Failed to mount '/dev/sdc1': Input/output error
NTFS is either inconsistent, or there is a hardware fault, or it's a
SoftRAID/FakeRAID hardware. In the first case run chkdsk /f on Windows
then reboot into Windows twice. The usage of the /f parameter is very
important! If the device is a SoftRAID/FakeRAID then first activate
it and mount a different device under the /dev/mapper/ directory, (e.g.
/dev/mapper/nvidia_eahaabcc1). Please see the 'dmraid' documentation
for more details.
```
solution:
```
sudo ntfsfix /dev/sdc1 # /dev/sdc1 is the partiion name of USB
```

7. USB read only

Root can write.


8. `unable to locate package texlive-full`

change apt source in `/etc/apt/sources.list`


9. `W: An error occurred during the signature verification. The repository is not updated and the previous index files will be used. GPG error: https://repo.mongodb.org/apt/ubuntu xenial/mongodb-org/3.6 Release: The following signatures were invalid: EXPKEYSIG 58712A2291FA4AD5 MongoDB 3.6 Release Signing Key <packaging@mongodb.com>`

see [here](https://itsfoss.com/failed-to-download-repository-information-ubuntu-13-04/)


10. ubuntu desktop is not loading after installing 18.04
```
systemctl start gdm
```

11. Cannot start the Ubuntu: `failed to start lsb create lightweight portable self-sufficient containers`:
+ root is full, remove some files in root directory (Ctrl+Alt+F2/3/4, and enter the ubuntu)

12. Restart yet the resolution is very low. The NVIDIA driver is not working.
```
sudo bash cuda_11.3.0_465.19.01_linux.run
```

13. Install mmcv-full failure: `pip install mmcv-full -f https://download.openmmlab.com/mmcv/dist/cu102/torch1.8.1/index.html` -> `ERROR: Failed building wheel for mmcv-full`

Solution: [ERROR: Failed building wheel for mmcv-full](https://github.com/open-mmlab/mmcv/issues/1055). The default cuda version does not match the script specification.
```
export PATH=/usr/local/cuda/bin:$PATH 
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
```

## 23. Python
1. 使用pickle的时候报错`UnicodeDecodeError: 'utf-8' codec can't decode byte 0x80 in position 0: invalid start byte`
    ```
    import pickle

    with open(path) as f:
        info = pickle.load(f)
        print(info)
    ```
    1. 原因: f编码错误,可以尝试decode等,但是这里的问题是open没有指定`rb`
    2. 解决
    ```
    import pickle

    with open(path, "rb") as f:
        info = pickle.load(f)
        print(info)
    ```
2. opencv
```
OpenCV Error: Unspecified error (The function is not implemented. Rebuild the library with Windows, GTK+ 2.x or Carbon support. If you are on Ubuntu or Debian, install libgtk2.0-dev and pkg-config, then re-run cmake or configure script) in cvShowImage, file /feedstock_root/build_artefacts/work/opencv-3.1.0/modules/highgui/src/window.cpp, line 545 Traceback (most recent call last): File "untitled.py", line 7, in cv2.imshow('image',img) cv2.error: /feedstock_root/build_artefacts/work/opencv-3.1.0/modules/highgui/src/window.cpp:545: error: (-2) The function is not implemented. Rebuild the library with Windows, GTK+ 2.x or Carbon support. If you are on Ubuntu or Debian, install libgtk2.0-dev and pkg-config, then re-run cmake or configure script in function cvShowImage
```
answer:
```
conda install -c menpo opencv3

```
3. use seleinum on CentOS7
```
selenium.common.exceptions.WebDriverException: Message: unknown error: DevToolsActivePort file doesn't exist
```
answer:
```
options.add_argument("--incognito")
options.add_argument('--headless')
options.add_argument('--no-sandbox')

```
4. `selenium.common.exceptions.ElementNotVisibleException: Message: element not interactable`
answer: 
+ xpath is incorrect
+ the page is not displayed/loaded fully: we can use `try-catch` and screenshot to see if it happens
```
# we can get the window size of the target page using 'window.innerWidth' and 'window.innerHeight' under the developer mode
browser.set_window_size(1853, 1053)
```
or
```
import selenium.webdriver.support.ui as ui
wait = ui.WebDriverWait(browser,100)
wait.until(lambda driver: driver.find_element_by_xpath('//figure/a[@href="/v/ac5005005"]/following-sibling::figcaption/p/span[@id="ticketBtn"]'))
```
+ too many chrome process
```
pkill chrome
```

## 24. Markdown
1. 为什么我同样使用下面代码,有的时候可以用,有的时候不能用(比如GitHub上`2.ghi`会是灰色的)?  =>  因为我的abc序号可能超过100了,如果是101的话就不行了
```
1. abc
    1. def
    ```
    ```
    2. ghi
    ```
    ```
```
## 25. PyTorch
1. `AttributeError: 'torch.LongTensor' object has no attribute 'data'`
    1. 解决: 把tensor转换成Variable
2. `RuntimeError: Trying to backward through the graph a second time, but the buffers have already been freed. Specify retain_graph=True when calling backward the first time.`
    1. 方法1
    ```
    y_pred, h_state = model(x, h_state)
    h_state = h_state.data
    ```
    2. 方法2 
    ```
    D_loss.backward(retain_graph=True)  # 如果在同一个循环体中后面还有G_loss.backward的话
    ```
3. `TypeError: can't convert CUDA tensor to numpy. Use Tensor.cpu() to copy the tensor to host memory first.`
    1. 原因: `xxx.cuda().data.numpy()`是不允许的, 也就是说不能对已经调用cuda()并且用data转换为tensor后的CUDA tensor使用numpy()
    2. 解决
        1. <方法1> 直接使用CUDA tensor进行计算,也就是说所有结果都用tensor计算(torch tensor有个属性是device,就是规定了它是cpu还是gpu)
        2. <方法2> 转换成cpu格式后进行计算,如`xx.cuda().data.cpu().numpy()`

4. GPU out of memory
    1. 可能是版本问题.我使用1.0.0的pytorch会出现这个问题,而使用0.3.1就没了
    
5. docker
```
RuntimeError: DataLoader worker (pid 720) is killed by signal: Bus error
```
原因: Docker开的shared memeory太少了
解决: 
    1. 方法1: 开docker的时候给更大的shared memory:https://github.com/pytorch/pytorch/issues/2244
    2. 方法2: 减小pytorch的num_worker


6. pyramid-box: `python train.py`
```
RuntimeError: merge_sort: failed to synchronize: an illegal memory access was encountered
```
原因:我发现loss会变得无穷大,但是有的时候运行没问题,而有的时候会出现问题.所以不是很清楚.多run几次总会有成功的- - 

7. loss.backward:
```
RuntimeError: select(): index 25 out of range for tensor of size [25, 32, 9956] at dimension 0
```
解决: `model.py`从
```
return y_predicted, captions, lengths, alphas
```
改为
```
return y_predicted, captions, decoder_lengths, alphas
```
因为`decoder_lengths = [length - 1 for length in lengths]`

这个report我猜测是因为我用过多的length去pack了一个变量,导致loss function去unpack这个变量的时候发现原来该有的位置没有了.比如说`y_predicted`的shape是`[32, 21, 9956]`,而`lengths=[22, 19, 17, 16, ... ,10]`,这就导致`lengths[0]=22`的地方`y_predicted`是没有的,这个时候用`lengths`去pack `y_predicted` , 然后用packed结果直接输入到loss function中就会导致这个问题,解决的方法也很简单,因为这个attention的image captioning是不考虑`<eos>`的输出,所以`y_predicted`的shape才会少1,那么lengths自然也少1,因为每个length都对应1个lstm timestep的output,所以可以解决这个问题


8. Image Captioning based on Attention: The inference process blows up memory
```
RuntimeError: CUDA out of memory. Tried to allocate 512.00 MiB (GPU 0; 10.91 GiB total capacity; 9.82 GiB already allocated; 431.38 MiB free; 20.20 MiB cached)
```
solution: pack the inference codes inside `with torch.no_grad` to prevent accumulating gradients. See details at [this](https://github.com/pytorch/pytorch/issues/235). See my code at [this](https://github.com/orris27/machine-learning-codes/blob/master/advanced_topics/image_captioning_attention/sample.py)
```
with torch.no_grad():
    features = encoder(imgs)
    captions = decoder.generate(features, vocab.word2idx['<sos>'])

```
    
    
9. `Cuda assert fails: device-side assert triggered`
+ out-of-bounds indexing
+ labels that are input to loss_fn have negative numbers
    
10. enumerate trainloader, reports: `an integer is required (got type tuple)`

Reason: variable-length vector

`__getitem__` pads the vector


11. `/usr/local/lib/python3.6/dist-packages/ipykernel_launcher.py:73: UserWarning: Using a target size (torch.Size([200, 1, 200])) that is different to the input size (torch.Size([200, 1])). This will likely lead to incorrect results due to broadcasting. Please ensure they have the same size.`

Reason: The shape of two arguments of `F.smooth_l1_loss` are different.

Solution: Reshape these two arguments to make them the same shape

12. `# !git checkout dev && git clean -d -f && git pull origin dev`

`train_loader = DataLoader(PSFDataset(train_names),  batch_size=args.batch_size, shuffle=True, collate_fn=collate_fn)`, the `train_names` is empty

13. `one of the variables needed for gradient computation has been modified by an inplace operation`

+ Debug methods, see [here](https://github.com/pytorch/pytorch/issues/15803), for example
```
with torch.autograd.set_detect_anomaly(True):
    a = torch.rand(1, requires_grad=True)
    c = torch.rand(1, requires_grad=True)

    b = a ** 2 * c ** 2
    b += 1
    b *= c + a

    d = b.exp_()
    d *= 5

    b.backward()

```
+ My case 1:
```
with torch.autograd.set_detect_anomaly(True):
    a = torch.rand(1, requires_grad=True)
    c = torch.rand(1, requires_grad=True)

    d = torch.zeros(3, dtype=torch.float)
    d[0] = a** 2
    d[1] = c ** 2
    d[2] = d[0] ** 2 # this cause inplace problem
    b = torch.mean(d)
    b.backward()
```

+ My case 2:
```
with torch.autograd.set_detect_anomaly(True):
    a = torch.rand(1, requires_grad=True)
    c = torch.rand(1, requires_grad=True)

    d = torch.zeros(2, 3, dtype=torch.float)
    for i in range(2):
        k = torch.zeros(3, dtype=torch.float)
        k[0] = a** 2 + i
        k[1] = c ** 2 + i
        k[2] = a ** 3+i
        d[i] = k
            
    for j in range(3):
        max_value = torch.max(d[:, i])   
        min_value = torch.min(d[:, i])
                                      
        d[:, i] = 2.0 * (d[:, i] - min_value) / (max_value - min_value) - 1.0
    b = torch.mean(d)                                                        
    b.backward()                                                             

```
solution
```
with torch.autograd.set_detect_anomaly(True):
    a = torch.rand(1, requires_grad=True)
    c = torch.rand(1, requires_grad=True)

    d = torch.zeros(2, 3, dtype=torch.float)
    for i in range(2):
        d[i, 0] = a ** 2 +i
        d[i, 1] = c ** 2 + i
        d[i, 2] = a ** 3 + i
    
    d1 = torch.zeros(2, 3, dtype=torch.float)        
    for j in range(3):
        max_value = torch.max(d)   
        min_value = torch.min(d)
                                      
        d1[:, i] = 2.0 * (d[:, i] - min_value) / (max_value - min_value) - 1.0
    b = torch.mean(d1)                                                        
    b.backward()        

```

14. `error: cholesky_cuda: U(1,1) is zero, singular U`

A matrix is singular if its determinant is zeros, in which case it has no inverse. In my codes, this error occurs because the elements in matrix are `NaN`.

## 26. Vue
1. `npm run dev`报错:
```
  ✘  http://eslint.org/docs/rules/no-multiple-empty-lines  More than 1 blank line not allowed  
  src/router/index.js:137:1
```
解决: 整个文件中不能有连续的空行
2. `npm run dev`报错:
```
  ✘  http://eslint.org/docs/rules/comma-dangle             Unexpected trailing comma           
  src/router/index.js:134:8
        },
```
解决: 最后一项不能在后面添加逗号

## 27. java
1. `multi-catch statement is not supported`
解决: `https://stackoverflow.com/questions/26940143/intellij-13-1-6-jdk-8-support`

注意: 如果reimport `pom.xml`的话会又变回去.所以还要添加下面的代码
```
  <build>
    <plugins>
      <plugin>
        <groupId>org.apache.maven.plugins</groupId>
        <artifactId>maven-compiler-plugin</artifactId>
        <version>2.3.2</version>
        <configuration>
          <source>1.8</source>
          <target>1.8</target>
          <encoding>UTF-8</encoding>
        </configuration>
      </plugin>
    </plugins>
  </build>
```
2. `Error:(15, 45) java: package org.apache.lucene.queryparser.classic does not exist`
解决: lucene-queries和lucene queryparser都是单独的包,需要重新导入: (以下是7.5.0版本),具体参考:https://mvnrepository.com/artifact/org.apache.lucene/lucene-analyzers-common
```
<!-- https://mvnrepository.com/artifact/org.apache.lucene/lucene-queries -->
<dependency>
    <groupId>org.apache.lucene</groupId>
    <artifactId>lucene-queries</artifactId>
    <version>7.5.0</version>
</dependency>

```
3. `Error:(28, 33) java: package org.wltea.analyzer.lucene does not exist`
解决: https://github.com/wks/ik-analyzer (直接粘贴下面的内容是没用的,这是国人写的,反正后来我就不准备解决这个问题了)
```
    <dependency>
      <groupId>org.wltea.ik-analyzer</groupId>
      <artifactId>ik-analyzer</artifactId>
      <version>3.2.8</version>
    </dependency>
```
## 28. NVIDIA
1. `nvidia-smi` reports:
```
NVIDIA-SMI has failed because it couldn't communicate with the NVIDIA driver. Make sure that the latest NVIDIA driver is installed and running.
```
只要执行`sudo apt-get install cuda-9-0`(直接打cuda的话默认是9.2,但是TF需要9.0,所以安装cuda-9-0)然后`reboot`就可以了.


## 29. git
1. `git merge origin/master` reports:
`fatal: refusing to merge unrelated histories`
直接
```
rm -rf .git # clear local history
git init

git remote add origin https://github.com/orris27/image-caption.git

git fetch origin 
git merge origin/master --allow-unrelated-histories
```
## 30. blockchain
1. `geth --datadir "/home/orris/.ethereum/" init ~/test-genesis.json` reports:
```
Fatal: invalid genesis file: invalid character 'Â' looking for beginning of object key string
```
原因: 使用了特殊的字符,尤其检查空格`/\ `的vim检查
2. `geth --datadir "/home/orris/.ethereum/" init ~/test-genesis.json` reports:
```
Fatal: Failed to write genesis block: genesis has no chain configuration
```
3. `There was an error fetching your accounts`
原因: 该服务器从`127.0.0.1:8545`中获取account信息了,而且这个`127.0.0.1:8545`不是服务器的地址,而是自己电脑的地址!因为我在自己电脑上运行ganache-cli(端口是8545)的时候,就不报这个错误了! 但是因为我的电脑没部署智能合约,所以不能起作用. 具体参考 [here](https://blog.csdn.net/victory_gwb/article/details/78288604)

解决: 在当前项目中查找`127.0.0.1`或者`8545`,然后发现app.js里会请求`127.0.0.1:8545`,把它改成服务器的ip地址和端口就可以了


## 31. vacant


## 32. ss
```
pkg_resources.DistributionNotFound: The 'shadowsocks==2.8.2' distribution was not found                                                                                                                                                       and is required by the application
shadowsocks start failed
```
说明默认的python版本不是2
```
ln -s /usr/bin/python2.7 /usr/bin/python
```

## 33. firewalld
1. 用firewalld添加端口的时候,执行`firewall-cmd --permanent --add-port=20-21/tcp`,报错如下所示:
```
Traceback (most recent call last):
  File "/usr/bin/firewall-cmd", line 24, in <module>
    from gi.repository import GObject
ModuleNotFoundError: No module named 'gi'
```
默认使用`/usr/bin/python`,如果我们在系统环境内装了python3,就不行了,开头改成`#!/usr/bin/python2 -Es`


## 34. http server
1. The browser cannot retreive <body> and keeps loading

reason: Content-Length in the response header is larger than the sent real length of response body

1. len = number_of_bytes("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><title>test.txt</title></head><body>") + number_of_bytes(file) + number_of_bytes("</body></html>"): If the file is empty, then len equals to 110

2. strlen(sendpacket) = number_of_bytes(response header) + len
```
strcat(sendpacket, "HTTP/1.0 200 OK\r\n");
strcat(sendpacket, "Server: MAXINYIN&&QUANYUQING\r\n");
strcat(sendpacket, "Content-length: ");
strcat(sendpacket, len);
strcat(sendpacket, "\r\n");
strcat(sendpacket, "Content-type: text/html\r\n\r\n");
strcat(sendpacket, "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><title>test.txt</title></head><body>");
index = strlen(sendpacket);
while ((ch = fgetc(file)) != EOF)
    sendpacket[index++] = ch;
strcat(sendpacket, "</body></html>");

if (send(fd, sendpacket, strlen(sendpacket), 0) < 0)
    printf("Fail in send.\n");

```
## 35. latex
1. `\begin{align*}` fail:

Make sure that there is no extra space (except the necessary newline) between the label and its contents.
```
# ok
\begin{align*}
y = x^2
\end{align*}

# no, since there is unecessary newline
\begin{align*}

y = x^2
\end{align*}
```
2. `LaTeX Error: File 'ctexrep.cls' not found.`
```
sudo apt-get install latex-cjk-all
```

3. arXiv cannot proceed eps file: See [LaTeX checklist for arXiv submissions](https://www.overleaf.com/learn/how-to/LaTeX_checklist_for_arXiv_submissions)

4. Align the caption for the subfigure: [problem description](https://tex.stackexchange.com/questions/331350/aligning-the-captions-for-the-side-by-side-subfigures). Solution: Add the `[t]` after the subfigure
```
\begin{subfigure}[t]{0.24\textwidth}
    \includegraphics[width=\linewidth]{figures/resnet18_cifar10_accuracy_nonconvex.pdf}
    \vspace{-0.2in}
    \caption{Testing Accuracy}
    \vspace{-0.1in}
\end{subfigure}
```

## 36. django
1. `python manage.py makemigrations` fails. 
```
  File "/media/orris/DATA/linux/gitrepo/recycle_book/bookmanage/admin.py", line 9, in <module>
    admin.site.register(Card)
NameError: name 'Card' is not defined

```

Answer: register models in `admin.py`

2. `makemigrations` reports:
```
Did you rename book.price to book.original_price (a DecimalField)? [y/N] y
You are trying to add a non-nullable field 'info' to book without a default; we can't do that (the database needs something to populate existing rows).
Please select a fix:
 1) Provide a one-off default now (will be set on all existing rows with a null value for this column)
 2) Quit, and let me add a default in models.py
Select an option: 2
```

Solution: Remove the old migration files:
```
mv bookmanage/migrations/0001_initial.py /tmp
```


3. `makemigrations` reports:
```
SystemCheckError: System check identified some issues:

ERRORS:
bookmanage.Order.buyer: (fields.E304) Reverse accessor for 'Order.buyer' clashes with reverse accessor for 'Order.seller'.
	HINT: Add or change a related_name argument to the definition for 'Order.buyer' or 'Order.seller'.
bookmanage.Order.seller: (fields.E304) Reverse accessor for 'Order.seller' clashes with reverse accessor for 'Order.buyer'.
	HINT: Add or change a related_name argument to the definition for 'Order.seller' or 'Order.buyer'.
```

Solution: Django is unable to generate unique names for a number of foreign keys in one table. Add related_name for them by ourselves: See details [here](https://stackoverflow.com/questions/2606194/django-error-message-add-a-related-name-argument-to-the-definition)
```
class Order(models.Model):
    buyer = models.ForeignKey('User',on_delete=models.CASCADE, related_name='user_buyer')
    seller = models.ForeignKey('User',on_delete=models.CASCADE, related_name='user_seller')
    book = models.ForeignKey('Book', on_delete=models.CASCADE, related_name='book_book')
```
4. `MultiValueDictKeyError at /book/add/`

```
img = request.FILES['photo'] 
```

Solution: See details [here](https://stackoverflow.com/questions/22831576/django-raises-multivaluedictkeyerror-in-file-upload)
+ enctype
+ id + name
```
<form action="{% url "bookmanage:book_add" %}" method="post" enctype="multipart/form-data">
<div class="custom-file">
    <input type="file" class="custom-file-input" name="photo" id="photo">
    <label class="custom-file-label" for="inputGroupFile04">选择这本书的封面</label>
</div>
```
5. makemigrations
```
/home/orris/anaconda3/lib/python3.6/site-packages/django/db/models/base.py:309: RuntimeWarning: Model 'bookmanage.book' was already registered. Reloading models is not advised as it can lead to inconsistencies, most notably with related models.
  new_class._meta.apps.register_model(new_class._meta.app_label, new_class)
No changes detected
```

Solution: Duplicate definition for one table. For example, define the class of `Book` twice

## 37. MQTT
1. I use TensorFlow model to predict image but failed. The MQTT does not give me any response.

predict globally to load model to GPU (This is the gpu problem of MQTT, and the professor tells me that the cpu version is not influenced)
```
model.predict(load_image('init.jpg')) # global
```
## 38. Others
Plug in the HDMI-VGA cable. The Ubuntu can find the output display, but the when I type `xrandr`, the output says "HDMI-1-1 disconnected": [answer](https://askubuntu.com/questions/796827/hdmi1-is-disconnected)
```
xrandr --output HDMI-1-1 --auto --right-of eDP1
```
