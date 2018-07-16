### 1. 启动
#### 1-1. mysqld_safe
数据库出问题了,一般用`mysqld_safe`来启动,因为可以带参数
```
sudo mysqld_safe --defaults-file=/data/3306/my.cnf &
```
#### 1-2. systemctl
```
sudo systemctl start mysqld.service
```

### 2. 优雅关闭
#### 2-1. mysqladmin
```
sudo mysqladmin -uroot -S /data/3306/tmp/mysql.sock shutdown -p
```
#### 2-2. systemctl
```
sudo systemctl stop mysqld.service
```
#### 2-3. kill(不推荐)
```
kill -USR2 $(cat xxx/pid)
```

### 3. 密码
#### 3-1. shell中修改密码
```
sudo mysqladmin -uroot password '123' -S /data/3307/tmp/mysql.sock # 一开始没有密码的时候
sudo mysqladmin -uroot -p'123456' password '123' -S /data/3307/tmp/mysql.sock # 要修改密码的话得先输入自己的密码
```
#### 3-2. 在mysql中修改自己密码
```
set password=password("new_password");
```
#### 3-3. 不用输入已知密码来修改密码
```
select user,host,authentication_string from mysql.user;
update mysql.user set authentication_string=password("123456789") where user='root' and host='localhost';
flush privileges;
```
#### 3-4. 找回密码
##### 注意
`kill -9`比killall更危险
##### 解决方法
1. 关闭原有的mysql服务进程
2. 启动mysql服务,这个服务可以不用密码就能登录mysql
3. 免密码登录mysql,并用3-3的方法修改密码
4. 退出
5. 重启mysql服务
```
sudo mysqladmin -uroot -poldpwd -S /data/3307/tmp/mysql.sock shutdown # 也可以pkill

sudo mysqld_safe --defaults-file=/data/3307/my.cnf --skip-grant-tables &

mysql -S /data/3307/tmp/mysql.sock
###
update mysql.user set authentication_string=password("123") where user='root' and host='localhost';
flush privileges;
exit;
###

sudo mysqladmin -S /data/3307/tmp/mysql.sock shutdown
sudo mysqld_safe --defaults-file=/data/3307/my.cnf  &
mysql -uroot -S /data/3307/tmp/mysql.sock -p
```
