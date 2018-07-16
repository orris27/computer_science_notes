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
最好password函数里用双引号,而不是单引号
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

### 4. 数据库
#### 4-1. 创建数据库
##### 4-1-1. 字符集
###### 1. 默认
+ 与自己的编译参数一致,否则就是latin1
+ collation是校对的.(具体我也不太清楚)
```
-DDEFAULT_CHARSET=utf8 \
-DDEFAULT_COLLATION=utf8_general_ci \
```
###### 2. utf8mb4
+ utf8mb4表示可以兼容4字节的Unicode编码.mb4表示most bytes 4.
+ utf8mb4是utf8的超集,支持emoji等,兼容utf8
+ 推荐使用utf8mb4,虽然浪费一定空间,但值得
###### 3. 支持的字符集
在编译时指定的`DEXTRA_CHARSETS`参数中就是支持的字符集
```
-DEXTRA_CHARSETS=gbk,gb2312,utf8,ascii \
```
###### 4. 指定字符集创建数据库
```
show create database db_default;
show create database db_default\G # 我在编译时指定了utf8编码和校对,所以这里也是utf8
# Create Database: CREATE DATABASE `db_default` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci */

create database db_gbk default character set gbk collate gbk_chinese_ci;
create database db_utf8 default character set utf8 collate utf8_general_ci;
# Create Database: CREATE DATABASE `db_utf8` /*!40100 DEFAULT CHARACTER SET utf8 */
```
#### 4-2. 查看数据库
##### 4-2-1. 所有,匹配
```
show databases;
show databases like '%db%'; # '%'表示匹配所有字符串
```
##### 4-2-2. 查看当前数据库
```
select database();
# other functions
select version();
select user();
select current_user();
select now(); # 显示时间
```

### 5. 权限
#### 5-1. 授权
grant 什么权限 on 哪个数据库的哪个表 to 哪个用户
+ 最好用户和主机都加引号
##### 5-1-1. grant
```
grant all on db_default.* to 'orris'@'localhost' identified by 'orris'; # 创建用户并授权
#<=> 
create user 'saber'@'localhost' identified by 'saber'; # 创建用户
grant all on db_default.* to 'saber'@'localhost'; # 授予用户权限
```
#### 5-2. 查看权限
```
help show grants
show grants;
show grants for current_user();
show grants for 'saber'@'localhost';
```

