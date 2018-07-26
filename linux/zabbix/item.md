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


## 2. 为自定义监控项添加图
### 2-1. 添加
1. 点击Configuration>Hosts>Graphs>Create Graph
2. 设置Name和Items就可以了
+ Name就取名key的名字,如`login-user`
+ Items可以同时添加多个item.选择`login-user`,设置Function等
### 2-2. 查看图
查看状态图一定是在Zabbix的Monitoring的Graphs里看的
+ 如果添加了最爱的话,Monitoring的首页也能看到

## 3. 为自定义监控项添加触发器
### 3-1. 添加触发器
当自定义监控项达到一定值时,报警.设置的方法和添加自定义监控项/添加图一样
1. 点击Configuration>Hosts>Triggers>Create Trigger
2. 设置Trigger信息
+ 触发器的名字取报警条件,如`login-user > 3`
+ 级别一般选择Warning
+ 触发条件(表达式)
- item(这里为`login-user`)
- Function(一般选择Last (most recent) T value >/=/< N)
- 设置N的值
### 3-2. 报警提示
在个人中心的Messaging里可以设置,按Play可以试听.一般设置High以上就好了

### 3-3. 处理问题
1. 如果报警条件满足,就会根据报警.在Problem栏里可以查看到问题
2. 点击Ack,可以填写处理信息=>方便未来查看当初的问题是如何解决的,以及团队合作等


## 4. 设置触发后的处理
### 4-1. 添加Media Type
1. 点击Administration>Media Types>Create media type
2. 设置action
+ Name比如说写`send_mail`
+ Zabbix的Email等类型都很辣鸡
+ Script name比如说写`send_mail.sh`

### 4-2. 在Zabbix的server端写脚本
1. 确定脚本写在哪个目录下
```
sudo cat /etc/zabbix/zabbix_server.conf | grep AlertScriptsPath
```
2. 在该目录下写脚本
```
cd /usr/lib/zabbix/alertscripts/
sudo echo 'echo $1 $2 $3 >> /tmp/test.log' | sudo tee send_mail.sh
sudo chmod +x /usr/lib/zabbix/alertscripts/send_mail.sh
```

### 4-3. 给当前用户添加新创建的Media
1. 点击Administration>User Groups>Zabbix Administrators里的Members栏(这个才是用户)
2. 点击导航栏的Media
3. 添加Media
+ 类型就是刚才取的Media名字
+ Send to写自己的邮件
+ 其他根据情况写

### 4-4. 设置Action
1. 点击Configuration>Actions>Name栏下的`Report problems to Zabbix administrators`
2. 在导航栏的Operations里edit Operations,在Send only to那里选择我们添加的media,如`send_mail`,点击内部的upgrade(不是外面的upgrade)
3. 在Configuration>Actions的状态栏处激活action


## 5. 创建模板
### 5-1. 在Configuration里创建模板
1. 点击Configuration>Templates>Create Template
2. 设置模板参数
+ Template name和Visible name比如说填`demo`
+ Groups选Templates
+ Host/templates将demo组下的主机,如`linux-node1`添加进来
3. 点击Add

### 5-2. 给模板添加监控项
在Configuration的Templates下点击对应模板的item就可以添加了
+ 添加item的设置和之前一样
### 5-3. 给模板添加1个application
在Configuration的Templates下点击对应模板的application就可以添加了
+ 只需要定义名字就可以了
### 5-4. 将item添加到application下
在Configuration的Templates下点击对应模板的items
+ 在Applications里添加进去


## 6. 克隆模板 
以`Template OS Linux`为例
### 6-1. 克隆本体
1. 在Configuration的Templates下点击对应模板
2. 点击Full Clone
3. 新出现的界面下面的选项没有clone按钮=>这是克隆的结果=>修改下名字,点击add就能成功克隆了
### 6-2. 克隆链接
1. 查看克隆出来的新模板是否有链接
2. 克隆链接的模板
3. 对于原来的模板,删除旧链接,改连接到新克隆出来的链接模板

## 7. 配置主动式agent
### 7-1. 修改agent的配置文件并重启
1. agent的StartAgents
2. ServerActive=服务器的ip
3. Hostname设proxy/server端的主机名
4. 重启
```
StartAgents=0
ServerActive=47.100.185.187
Hostname=linux-node1.example.com
sudo systemctl restart zabbix-agent
```

### 7-2. 获得type为Zabbix agent(active)的模板
以`Template OS Linux`为例
### 7-2-1. 克隆原来的模板/新建模板
克隆`Template OS Linux`及其链接的模板`Template App Zabbix Agent`,并将`Template OS Linux`的链接指向新的
### 7-2-2. 批量修改新模板的类型,使允许主动式agent
1. 选中模板的items,全选所有item,点击Mass Update
2. 在批量修改界面中,Type改为`Zabbix agent(active)`并保存
3. 两个模板都进行操作

### 7-2. 在server端添加agent
### 7-2-1. 添加新主机
和普通的一样添加就行了
> https://github.com/orris27/orris/blob/master/linux/zabbix/installation.md
### 7-2-2. 使用Active版的模板
在新主机上添加模板`Template OS Linux Active`


## 8. zabbix-proxy
zabbix-proxy的配置类似于zabbix-server的配置方法
> https://github.com/orris27/orris/blob/master/linux/zabbix/installation.md
### 8-1. 通过yum安装zabbix-proxy,zabbix-proxy-mysql(,mysql-server:如果之前没安装过MySQL的话)
### 8-2. 为zabbix-proxy创建数据库
1. 创建zabbix_proxy的数据库
2. 添加执行用户
3. 导入zabbix-proxy的schema(proxy版本不用导入其他表)
```
create database zabbix_proxy character set utf8 collate utf8_bin;
use zabbix_proxy
grant all privileges on zabbix_proxy.* to zabbix@localhost identified by 'zabbix';
source ~/tools/zabbix-3.4.11/database/mysql/schema.sql
```

### 8-3. 配置zabbix-proxy的配置文件并启动
1. ProxyMode=0=>主动式proxy.就是proxy主动跟老大说我要干活,然后老大安排它干活;之后proxy说其实我自己不干活,我叫agent干活(当然agent也分主动/和被动)
2. 如果是主动的话,Server=proxy-server的ip
3. Hostname一定要写自己的主机名,因为server会用这个来区分
4. 设置数据库相关
+ name
+ user
+ password
5. 启动zabbix-proxy服务
```
sudo find / -type s -name 'mysql.sock' # mysql.sock的类型是s,不是普通文件
sudo vim /usr/local/etc/zabbix_proxy.conf # 我是编译安装的,所以这里的位置不一样
###
ProxyMode=0
Server=172.19.28.82 # server端的ip地址
Hostname=linux-node1.example.com # 名字必须和Web上Create proxy时的proxy名字相同
DBName=zabbix_proxy
DBUser=zabbix
DBPassword=zabbix
###
sudo ln -s /usr/local/mysql/run/mysql.sock /tmp
sudo zabbix_proxy
```

### 8-4. 配置zabbix-server
#### 8-4-1. 点击Administration>Proxies>Create proxy
+ Proxy Name在生产环境上设置有意义的名称,为proxy端设置的Hostname
+ Proxy mode设置为active(根据自己的proxy端情况设置)
+ 主机暂时不用添加(在添加主机时,下面的proxy选项会添加)

#### 8-4-2. 添加主机(监听对象)
+ 修改为proxy模式
+ 添加模板
#### 8-4-3. 将agent端指向proxy端
修改agent配置文件,使指向proxy的IP
1. ServerActive(Server也可以考虑改下)改为proxy端IP
+ agent配置文件中ServerActive表示Zabbix servers and Zabbix proxies for active checks.
2. Hostname改为proxy端的域名

### 8-0. 问题
#### 8-0-1. 84的主机配置好主动agent和proxy后,server端还是接收不到IP为84的信息
查看proxy日志发现`cannot send proxy data to server at "172.19.28.82": proxy "linux-node3.example.com" not found`
##### 8-0-1-1. 解决
将proxy配置文件中的Hostname等于Web上创建的proxy的名字,并重启


## 9. 自动化监控(auto register)
### 9-1. agent端:修改agent配置文件并重启agent
#### 9-1-1 修改agent配置文件
+ `HostMetadataItem=system.uname`(用来给server端识别并判断我是哪种操作系统的)
+ 也可以修改HostMetadata
#### 9-1-2. 重启agent服务
### 9-2. server的web端:
点击Configuration>Actions>Auto Register>Create Action
#### 9-2-1. Action:
+ 取名(随意)
+ 条件(筛选主动请求server的zabbix服务器)
- 选择proxy(因为要自动注册的服务器是proxy模式)
- metadata like 'linux'(获取到的数值有linux) 添加
#### 9-2-2. Operations
+ Add host
+ Add to host groups: Linux servers
+ Link to templates: Template OS Linux Active

### 9-3. 等待
因为我们这里配置改来改去,可能发现会比较慢.可以尝试重启agent和proxy等.



### 9-0. 问题
#### 9-0-1. 84上的proxy报错`cannot send list of active checks to "172.19.28.84": host [linux-node3.example.com] not found`;而agent报错`no active checks on server [172.19.28.84:10051]: host [linux-node3.example.com] not found`
如果是server&agent的情况:agent上的Hostname要和server端在Web上配置的Host的名字要相同
##### 9-0-1-1. 解决
1. 在`/etc/hostname`上改成自己的主机名.但还是没有
2. 后来`pkill`了zabbix_proxy,多`pkill`了好几次,再重启zabbix_proxy,终于有用了
3. 结果到头来不知道为什么就好了




## 10. 配置被动式agent
### 10-1. 修改agent的配置文件并重启
1. agent的StartAgents改成3 
2. Server=server端服务器的ip
3. Hostname不用变
4. ServerActive不需要,改成默认的127.0.0.1
5. 重启
```
StartAgents=3
ServerActive=127.0.0.1
Hostname=linux-node1.example.com
Server=172.19.28.82
sudo systemctl restart zabbix-agent
```


## 11. 自动化监控(discovery)
server端主动通过一些条件在指定IP段内寻找适合条件的agent,如果找到了,就执行Operations里定义的操作
### 11-1. 点击Configuration>Discovery
#### 11-1-1. 修改默认的/新建项,并重启
这里选择修改默认的disabled了的项
+ Name随意
+ ip端改成要搜寻的ip端,范围越小越容易发现,如`172.19.28.83-84`
+ Delay改成60
+ Checks就是`system.uname`
+ 以ip地址作为唯一标识
### 11-2. 将事件的来源改成Discovery
点击Configuration>Action,将Event Source改成Discovery(可以考虑把其他的给disable掉)
### 11-3. 修改默认的/添加新的Discovery,并enable
+ Conditions没有特别要改的地方
+ Operations改成和自动化监控(auto register)一样
### 11-4. 在Monitoring>Events和Discovery可以查看是否发现了新主机
一般等待一会儿就能出现了

## 12. 自动化监控(API)
> https://www.zabbix.com/documentation/3.4/manual/api
### 12-1. 登陆server端的Web
+ curl的json数据的右大括号前面不能有逗号,即~~"id":1,~~,内嵌的大括号内也不能出现最后的逗号
+ 传输的格式参考官方文档
+ `python -mjson.tool`用来将返回的json变成易读的模式
```
curl -s -X POST -H 'Content-Type:application/json' -d '
{
    "jsonrpc": "2.0",
    "method": "user.login",
    "params": {
        "user": "Admin",
        "password": "zabbix"
    },
    "id": 1
}' http://47.100.185.187/zabbix/api_jsonrpc.php  | python -mjson.tool
### 返回的结果如下
{
    "jsonrpc": "2.0",
    "result": "3a9e5445570f8f53dfc4f1b364ec469f", # 这里就是我们要的tokens
    "id": 1
}
###
```
### 12-2. 添加主机
+ "host":添加的主机名
+ "type":1表示zabbix agent
+ "useip":1表示使用ip
+ "ip"填写agent的ip
+ "groups"下的"groupid"在Configuration>Host Group下,把鼠标悬停在`Linux Server`上,然后浏览器左下角中就会显示这个主机组的id;看url也行
+ "templateid"获取方式和`groupid`同理
+ "auth"填写之前登录时出现的"result"的值
```
curl -s -X POST -H 'Content-Type:application/json' -d '
{
    "jsonrpc": "2.0",
    "method": "host.create",
    "params": {
        "host": "Linux server",
        "interfaces": [
            {
                "type": 1,
                "main": 1,
                "useip": 1,
                "ip": "172.19.28.84",
                "dns": "",
                "port": "10050"
            }
        ],
        "groups": [
            {
                "groupid": "2"
            }
        ],
        "templates": [
            {
                "templateid": "10001"
            }
        ]
    },
    "auth": "3a9e5445570f8f53dfc4f1b364ec469f",
    "id": 1
}' http://47.100.185.187/zabbix/api_jsonrpc.php  | python -mjson.tool
### 返回结果如下
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "hostids": [
            "10266"
        ]
    }
}
###
```



0. 问题
0-1. `Invalid JSON. An error occurred on the server while parsing the JSON text.`
curl的json数据的右大括号前面不能有逗号,即 ~~"id":1,~~ ,内嵌的大括号内也不能出现最后的逗号





## 0. 问题
### 0-1. 设置好action后不执行?
查看Dashboard对应的Problem里的Actions提示,里面会有错误提示,如`Cannot exectue command "/usr/lib/zabbix/alertscripts/send_mail.sh": [13] Permission denied`
+ `sudo chmod +x /usr/lib/zabbix/alertscripts/send_mail.sh`
