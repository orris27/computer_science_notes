## 配置
### 1. 服务端
#### 1-1. 编辑配置文件
rsyncd的配置文件是`/etc/rsyncd.conf`.
+ `[shared]`为模块名
+ 模块名下的`path = /shared/`表示分享出来的文件夹
+ `auth users`表示允许以什么虚拟用户的身份访问,这个认证用户是假的,不用我们用`useradd`添加
+ `secrets file`表示存储允许登录的用户和密码的文件
+ `uid`,`gid`表示在共享文件夹中以什么身份访问  
+ pid file表示记录该服务器的进程号,如12780
+ 如果没该配置文件,就自己创建`/etc/rsyncd.conf`
+ `exclude`:排除a和b,无论在push/pull都会无视,但是不灵活
```
uid = rsync
gid = rsync
use chroot = no
max connections = 200
timeout = 300
pid file = /var/run/rsyncd.pid
lock file = /var/run/rsyncd.lock
log file = /var/run/rsyncd.log

# module
[shared]
path = /shared/ # the shared directory of this module
ignore errors
read only = false
list = false
hosts allow = 172.16.55.0/24
hosts deny = 0.0.0.0/32
auth users = rsync_backup # the user granted to access the shared directory
secrets file = /etc/rsyncd.password # the password of the user.Format:"username:password"

#exclude=a b
```

#### 1-2. 创建共享目录
```
mkdir /shared
```

#### 1-3. 添加服务所需的用户/组和目录
```
useradd rsync -s /sbin/nologin
chown -R rsync.rsync /shared
```

#### 1-4. 添加访问共享目录的用户/组
```
echo "rsync_backup:orris">/etc/rsyncd.password
chmod 600 /etc/rsyncd.password
```

#### 1-5. 启动rsync服务端
默认端口873
```
sudo rsync --daemon
```


### 2. 客户端
#### 2-1. 安装rsync
#### 2-2. 配置密码
```
echo "orris">/etc/rsync.password
chmod 600 /etc/rsync.password
```
#### 2-3. 使用
+ 使用方法=普通的scp/rsync,只是要用`::`
+ 密码<=交互/非交互(命令行/文件)
+ 使用模块的认证用户登录,区分普通用户用`::`两个冒号
+ `--no-o`,`--no-g`:表示不保留owner和group,而`-a`则隐式地包含了要保留owner,group等,所以这里显示指出报错说`rsync: chgrp "xxxx" (in shared) failed: Operation not permitted (1)`
```
rsync -avz --no-o --no-g rsync_backup@172.16.55.136::shared /data
rsync -avz --no-o --no-g rsync_backup@172.16.55.136::shared /data --password-file=/etc/rsync.password
rsync -avz --no-o --no-g /data/ rsync_backup@172.16.55.136::shared --password-file=/etc/rsync.password
```


