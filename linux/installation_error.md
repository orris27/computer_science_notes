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
## 2. Haproxy
### 2-1. etcd版本的SaltStack推送haproxy配置文件
1. 推送完后启动haproxy失败
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
