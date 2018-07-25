## 1. 添加自定义的监控项
我们这里以同时登录的人数为例
### 1-1. 通过命令获得监控值
```
uptime | awk '{print $6}'
```

### 1-2. 将获得监控值的命令添加到agent的配置文件里,并重启agent服务
在Zabbix的agent端上,UserParameter的语法是`key,command`
```
sudo vim /etc/zabbix/zabbix_agentd.conf
###
UserParameter=login-user,uptime | awk '{print $6}'
###
sudo systemctl restart zabbix-agent
```
### 1-3. 在zabbix的server端上看能不能获取到这个key
zabbix_get的`-s`参数要外网IP地址,如果写内网IP地址,就会报错`zabbix_get [9213]: Check access restrictions in Zabbix agent configuration`
```
sudo yum install zabbix-get -y
zabbix_get --help
zabbix_get -s 47.100.185.187 -k login-user
```
### 1-4. 在zabbix的web端上,给对应的被监控主机添加监控项
1. 点击Configuration>Hosts>items>Create item
2. name和key最好都写一样.这里的key就是之前在agent端的配置文件里配置的key.所以是`login-user`
3. `Type of information`填获得的值的数据类型,如整数,浮点数,字符串等
4. `Update interval`填写监控的间隔.不是特别重要的参数就5分钟,否则就30s/60s
5. `Custom intervals`为高级定义,如指定某个星期的什么时间段内监测间隔为30s,而另外的时间段监测间隔为300s等
6. `New application`可以给这个item添加到某个application里面
7. `Populates host inventory field`可以将item放到某个资产里,随便填

### 1-5. 在items界面上查看状态
如果是Enabled就ok了
