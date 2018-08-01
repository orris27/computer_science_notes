## 1. 安装(master-minion)
1. 在master端安装master和minion两个版本
```
sudo yum install salt-master salt-minion -y
```
2. 在minion端只用安装minion版本
```
sudo yum install salt-minion -y
```

3. master端启动master服务
```
sudo systemctl start salt-master
sudo systemctl enable salt-master
```
4. 在master端服务器和minion端配置minion,告诉minion master在哪里.然后启动minion服务
+ master:172.19.28.82 (或者改成域名也可以)
+ id:可以不配,或者根据业务类型配置.这里就先不配了
```
sudo vim /etc/salt/minion
###
master: 172.19.28.82
#id:
###
sudo systemctl start salt-minion
sudo systemctl enable salt-minion
```
5. 认证(命令传输加密的)
5-0. 目录结构
+ /etc/salt/pki/minion/目录下有minion的秘钥和公钥(第一次启动minion服务的时候生成)
+ /etc/salt/pki/master/目录下有master的秘钥和公钥,
+ minions_pre目录下有等待同一的minion服务器的公钥
```
tree /etc/salt
```
5-1. 查看等待的key并确认
+ salt-key查看keys的状态,默认加了`-L`
+ `-A`:同意所有
+ `-a linux*`:指定对象,通过Y来确定是否同意,支持通配符
+ master端同意完后,也把自己的公钥放到了minion端的minions目录下
+ minions下为同意了的minion端的公钥
```
salt-key
# salt-key -A
salt-key -a linux* 
tree /etc/salt
```

6. 远程执行
> https://github.com/orris27/orris/blob/master/linux/shell/command.md
```
salt '*' test.ping
### 返回结果
linux-node3.example.com:
    True
linux-node1.example.com:
    True
###
```

## 2. 安装(Syndic)
从左到右分别是从属关系:172.19.28.82(minion)+172.19.28.84(minion)=>172.19.28.84(master+syndic)=>172.19.28.84(高级master)
1. yum安装master和Syndic
+ syndic必须依赖于master
```
sudo yum install salt-master salt-syndic -y
```
2. 在syndic端指定高级master
+ 在修改Syndic端的master配置文件
```
sudo vim /etc/salt/master
######################
syndic_master: 172.19.28.82
######################
```
3. 启动服务
    1. salt-master
    2. salt-syndic
```
sudo systemctl start salt-master
sudo systemctl start salt-syndic
```
4. 提醒高级master自己已经是高级master,而不是原来普通的master
```
############################################
# 在高级master,也就是172.19.28.82里设置
############################################

sudo vim /etc/salt/master
####################
order_masters: True
####################
sudo systemctl restart salt-master
```
5. 如果之前的环境有master-minion相关参数,就要初始化
    1. 暂停之前的minion
    ```
    ##############################
    # 在两台机器上都执行
    ##############################
    sudo systemctl stop salt-minion
    ```
    2. 清除之前master的key
    ```
    ##############################
    # 在之前的master环境删除(172.19.28.82)
    ##############################
    salt-key
    salt-key -D
    ```
    3. 删除之前的公钥的实体文件
    ```
    ############################
    # 两台机器都执行
    ############################
    cd /etc/salt/pki/minion
    rm -f *
    ```
    4. 可能要清除minion_id
    ```
    cd /etc/salt/pki
    cat minion_id # 这里暂且不删除
    ```
6. 给Syndic认几个小弟
+ 根据我们的架构,82和84都是84的小弟,所以这里的minion就要配置
```
###################################
# 在82和84上都执行
###################################



sudo vim /etc/salt/minion
####################
master: 172.19.28.84
####################
sudo systemctl start salt-minion
```
7. 建立从属关系
    1. 高级master收低级master为徒
    2. 低级master(syndic)收minion为徒
```
####################################
# 172.19.28.84
####################################
salt-key
####################################
# 172.19.28.82
####################################
salt-key # 如果下面有84的id,那么这就是syndic
salt-key -A

####################################
# 172.19.28.84
####################################
salt-key -A

```
8. 检测
```
####################################
# 172.19.28.82
####################################
salt '*' test.ping
```
