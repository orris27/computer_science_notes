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
默认是对所有数据库的usage权限
#### 5-1. 授权
grant 什么权限 on 哪个数据库的哪个表 to 哪个用户
+ 最好用户和主机都加引号create database eru_db default character set utf8mb4 collate utf8mb4_general_ci;
```
grant all on db_default.* to 'orris'@'localhost' identified by 'orris'; # 创建用户并授权
#<=> 
create user 'saber'@'localhost' identified by 'saber'; # 创建用户
grant all on db_default.* to 'saber'@'localhost'; # 授予用户权限
```
#### 5-2. 查看权限
##### 5-2-1. 查看用户的权限
```
help show grants
show grants;
show grants for current_user();
show grants for 'saber'@'localhost';
```
##### 5-2-2. 查看权限种类
SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, REFERENCES, INDEX, ALTER, CREATE TEMPORARY TABLES, LOCK TABLES, EXECUTE, CREATE VIEW, SHOW VIEW, CREATE ROUTINE, ALTER ROUTINE, EVENT, TRIGGER
1. 授予一个all权限给一个用户,然后revoke一个全新啊,这个时候再查看她的用户权限,就会有其他权限出来了
```
show grants for megumi@'localhost';
# revoke insert on *.* from 'megumi'@'localhost'; # 如果当初是给数据库db_megumi授权的话,撤回权限也要对应这个数据库,而不能写*.*
revoke insert on db_megumi.* from 'megumi'@'localhost';
mysql -uroot -S /data/3307/tmp/mysql.sock  -e 'show grants for megumi@localhost' -p | grep GRANT | tail -1 # -e可以不进入数据库执行sql语句
```
2. mysql.user表里的属性
```
select * from mysql.user\G # \G可以按列查看
```
#### 5-3. 分配权限原则
最小化.一般增删改查就可以了,建/删表都由DBA处理
+ create/drop一般由DBA管理
```
grant insert,delete,update,select on db_mirai.* to 'mirai'@'172.19.28.%' identified by 'mirai';
```



### 6. 远程连接
+ shell中mysql的`-h`参数指定连接哪台主机,和数据库里的host不一样.数据库里的host指的是哪些主机可以访问,而这个在我们登录时自动提供了.我们只需要说明远程登录哪台主机就可以了
#### 6-1. 服务器
需要创建用户,这个用户的host表明哪些ip可以访问
```
create user 'eru'@'172.19.28.%';
```
#### 1. 多实例
不用sock登录,而是直接指定端口就可以了
```
mysql -ueru -h'172.19.28.82' -P3307
mysql -ueru -h'172.19.28.82' -P3307 -p
````
#### 2. 单实例
```
create database eru_db default character set utf8mb4 collate utf8mb4_general_ci;
grant all on eru_db.* to 'eru'@'172.19.28.%' identified by 'eru';
mysql -ueru -h'172.19.28.82' -p
```
### 7. 查看语句的详细内容
```
show xxx \G
# show create database db_default\G
# show create table student\G
```

### 8. 表
#### 8-1. 创建表
##### 字符集
+ 默认继承数据库的字符集
```
create table fun1(id int);
show create table fun1\G
#Create Table: CREATE TABLE `fun1` (
#  `id` int(11) DEFAULT NULL
#) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci # 我的数据库默认是utf8mb4
```
##### 引擎
数据库存储数据的方式,5.1以前是MyISAM,5.5以后是InnoDB(默认引擎)
##### 参数
1. NOT NULL
2. DEFAULT 给定默认值
3. COMMENT 注释
4. auto_increment 需要在属性和结尾处都写上,结尾默认是1(从1开始)
+ 建表结尾的auto_increment赋值是表示从什么数开始+1往上.
+ 插了不同数据后,`show create table fun6\G`显示的auto_increment也会发生变化
+ `\G`表示按列显示
```
# 建立一个以100,101,..的方式创建pk的表fun6
create table fun6(id int auto_increment,name varchar(20),primary key(id)) auto_increment=100;
```
##### 数据类型
1. tinyint(1) 表示用1个字节来存储整形,只能存储-128~127.如果插入一个128的数据的话,就会弹出Out of range并禁止行为.
##### 生产环境
某sns产品的生产正式建表语句
```
use sns;
set names gbk;
CREATE TABLE `subject_comment_manager` (
  `subject_comment_manager_id` bigint(12) NOT NULL auto_increment COMMENT '主键',
  `subject_type` tinyint(2) NOT NULL COMMENT '素材类型',
  `subject_primary_key` varchar(255) NOT NULL COMMENT '素材的主键',
  `subject_title` varchar(255) NOT NULLL COMMENT '素材的名称',
  `edit_user_nick` varchar(64) default NULL COMMENT '修改人',
  `edit_user_time` timestamp NULL default NULL COMMENT '修改时间', 设置时间戳,方便记录
  `edit_comment` varchar(255) default NULL COMMENT '修改的理由',
  `state` tinyint(1) NOT NULL default '1' COMMENT '0 代表关闭,1 代表正常',
  PRIMARY KEY (`subject_comment_manager_id`), # 貌似不能加引号
  KEY `IDX_PRIMARYKEY` (`subject_primary_key`(32)), #括号内的 32 表示对前 32 个字符做前缀索引。
  KEY `IDX_SUBJECT_TITLE` (`subject_title`(32)),
  KEY `index_nick_type` (`edit_user_nick`(32),`subject_type`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
```
#### 8-2. 查看表结构
```
desc stu; # describe
```

