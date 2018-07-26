## 1. 安装
1. 在master端安装master和minion两个版本
```
sudo yum install salt-master salt-minion -y
```
2. 在minion端只用安装minion版本
```
sudo yum install salt-minion -y
```

3. master端启动master服务
4. 在master端服务器和minion端配置minion,告诉minion master在哪里.然后启动minion服务
+ master:172.19.28.82 (或者改成域名也可以)
+ id:可以不配,或者根据业务类型配置.这里就先不配了
5. 认证(命令传输加密的)
/etc/salt/pki/minion/目录下有minion的秘钥和公钥(第一次启动minion服务的时候生成)
/etc/salt/pki/master/目录下有master的秘钥和公钥,
minions_pre目录下有等待同一的minion服务器
salt-key查看keys的状态,默认加了`-L`
`-A`:同意所有
`-a linux*`:指定对象,通过Y来确定是否同意,支持通配符
master端同意完后,也把自己的公钥放到了minion端的minions目录下
minions下为同意了的minion端的公钥
6. 远程执行
`salt '*' test.ping`
salt是命令,`'*'`单引号防止转义,`*`表示所有
test.ping是SaltStack用来检测能不能和minion相通
test是一个模块
ping是一个方法




