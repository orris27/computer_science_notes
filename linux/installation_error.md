## 1. `error while loading shared libraries: libpcre.so.1: cannot open shared object file: No such file or directory`
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
