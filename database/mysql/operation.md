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
#### 问题
1. 启动不了,说没有pid文件
+ 配置文件出错了,可以通过看错误日志,然后发现哪个配置语句有问题

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
#### 4-0 登录数据库
```
mysql -uroot -S /data/3307/tmp/mysql.sock -p
# alias mysql='mysql -S /data/3307/tmp/mysql.sock' # 设置别名,免去写sock的麻烦
# mysql -uroot -p
```
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
#### 8-3. 插入
##### 同时插入多个数据
```
insert into test values(1,'orris'),(2,'mirai');
```
#### 8-4. 清空
1. `delete from test`
2. `truncate test`

##### 分析
truncate更快,因为truncate直接删除物理文件,而delete from只是逻辑删除(比如Lazy Deletion),所以还要一行一行搜索
#### 8-5. 属性
##### 1. 添加属性
###### 第一位
```
alter table test add first tinyint first;
```
###### 中间
```
alter table test add mid timestamp after name;
```
###### 最后
```
alter table test add last int;
```
##### 2. 修改属性
如果修改类型的话,不兼容的类型是不被允许的
```
alter table test change last new_last bigint;
```
##### 3. 删除属性
```
alter table test drop new_last;
```
#### 8-6 表的重命名
```
rename table test to thank;
```



### 9. 索引
#### 9-1. 索引种类
##### 主键索引
建表时如果指定主键,那么就自动生成主键索引.
##### 普通索引
可以创建/添加普通索引
##### 联合索引
+ 可以创建/添加联合索引
+ 具有前缀性.如abc建立的话,只能ab,abc,而不能ac,bc
##### 唯一索引(和上面的不是一个分类标准)
如果对一个属性创建唯一索引,那么她就变成unique了,即使原来允许重复的值,只要建立了唯一索引,就不能插入重复的值了
#### 9-2 操作
##### 创建
```
create table index1(id int,primary key(id));
create table index2(id int, name varchar(20),key index_name(name)); # key 索引名字(哪个属性)
create table index3(id int, name varchar(20),age tinyint,key index_name_age(name,age)); # 联合索引
```
##### 添加(alter+create)
```
alter table index2 change id id int primary key auto_increment; # 给index2表的id属性添加主键索引(不推荐)
alter table index2 add index index_name(name(10)); # 如果name为很长的字符串,比如一篇文章,那么就指定前10个字符来建立索引

create index index_name on index2(name);
create unique index index_name on index2(name);
```
##### 删除(alter+drop)
```
alter table index2 drop index index_name;

drop index index_name on index2;
```
##### 查看
```
desc table1; # 查看KEY列,PRI=主键索引;UNI=唯一索引(不允许重复)
show index from table1\G;
```
结果
```
*************************** 1. row ***************************
        Table: index1
   Non_unique: 0 # 是否为唯一索引
     Key_name: PRIMARY # 索引名字,我们可以在这里查看
 Seq_in_index: 1
  Column_name: id
    Collation: A
  Cardinality: 0
     Sub_part: NULL
       Packed: NULL
         Null: 
   Index_type: BTREE # 索引类型,默认为b树
      Comment: 
Index_comment: 
```
#### 9-3 选择
+ 读取少而写入快的不要建立索引
+ 表小的,只有几百行的不要建立索引
+ 尽量在唯一值多的大表上建立索引

### 10. 备份
数据库的备份采用逻辑备份的方法,即导出sql语句
#### 逻辑备份
备份实例为3307的MySQL服务下的db_default这个数据库的数据到/opt/db_default_bak.sql
```
mysqldump -uroot -B db_default -S /data/3307/tmp/mysql.sock -p >/opt/db_default_bak.sql 
# sudo mysqldump -uroot -B db_default -S /data/3307/tmp/mysql.sock -p | sudo tee /opt/db_default_bak.sql
```
备份MySQL实例的所有数据库(`-A`表示所有数据库)
```
sudo mysqldump -uroot -A -S /data/3307/tmp/mysql.sock -p | sudo tee /opt/all_bak.sql
```
查看导出的sql语句
```
egrep -v '#|\/|^$|--' /opt/db_default_bak.sql
```
#### 增量备份
1. 我们不能实时逻辑备份,但我们可以让MySQL自己实时记录我们的每一个更改操作
2. 数据库需要恢复一般是某个员工的一个错误操作
3. 在我们逻辑备份后的一段时间内,如果某个瓜皮搞事的话,我们就要恢复数据
4. 通过MySQL记录的增量备份,我们可以看到这段时间内的所有语句.先逻辑备份后增量备份就好了
5. 但是我们不能保证这一期间没有其他操作.比如逻辑备份期间如果也会有记录,增量备份中不能有它.要么停止,要么换个位置,要么我们提前拿出来
##### 工具
1. mysqlbinlog 导出/查看增量备份文件(二进制文件)
```
mysqlbinlog -d db_default mysqlbin_orris.000001 > /opt/db_default_bin_bak.sql # -d指明哪个数据库
```
##### 实战
如果将一个表的name属性全变成orris的话,我发现增量备份里面没有这个语句.不知道为什么.
1. 先备份数据库db_default
2. 打开log-bin配置
3. 重启MySQL服务
4. 各种操作+骚操作
5. 让MySQL换个地方写增量
6. 在增量备份中去掉之前那个骚操作
7. 逻辑备份+增量备份
```
mysqldump -uroot -B db_default -S /data/3307/tmp/mysql.sock -p >/opt/db_default_bak.sql

sudo vim /data/3307/my.cnf
###
log-bin=/data/3307/logs/mysqlbin_orris
###

sudo mysqladmin -uroot -S /data/3307/tmp/mysql.sock shutdown -p
sudo mysqld_safe --defaults-file=/data/3307/my.cnf &

mysqladmin -S /data/3307/tmp/mysql.sock -uroot -p flush-log

mysqlbinlog -d db_default mysqlbin_orris.000001 > /opt/db_default_bin_bak.sql

sudo vim /opt/db_default_bin_bak.sql

mysql -uroot -S /data/3307/tmp/mysql.sock  db_default -p</opt/db_default_bak.sql
mysql -uroot -S /data/3307/tmp/mysql.sock  db_default -p</opt/db_default_bin_bak.sql
```

### 11. 还原
通过备份的sql语句来还原
```
mysql -uroot -S /data/3307/tmp/mysql.sock  db_default -p</opt/db_default_bak.sql
```


### 12. 排序
1. 指定谁
2. 怎么排(asc,desc)
```
select * from index2 order by id asc;
select * from index2 order by id desc;
```
### 13. 限制结果行数
#### 思路
1. 前面几行
2. <开始,结束>
3. <开始,个数>
```
select * from index2 limit 2; # 前2行
select * from index2 limit 3,5; # 第3行开始的5个元组
```
### 14. 分析
MySQL中的explain可以分析某个语句的执行计划
#### 例子
##### 没有索引
```
explain select * from index2 where name ='orris'\G
```
结果
```
*************************** 1. row ***************************
           id: 1
  select_type: SIMPLE
        table: index2 # 查询什么表
   partitions: NULL
         type: ALL
possible_keys: NULL # 是否有可用的索引使用,NULL表示没有
          key: NULL
      key_len: NULL
          ref: NULL
         rows: 4 # 共查询了多少元组才停止
     filtered: 25.00
        Extra: Using where
1 row in set, 1 warning (0.00 sec)
```
#### 有索引
```
create index index_name on index2(name);
explain select * from index2 where name ='orris'\G
```
结果
```
*************************** 1. row ***************************
           id: 1
  select_type: SIMPLE
        table: index2
   partitions: NULL
         type: ref
possible_keys: index_name
          key: index_name
      key_len: 83
          ref: const
         rows: 1
     filtered: 100.00
        Extra: Using index
1 row in set, 1 warning (0.01 sec)
```

### 15. 安全模式
由于表更新时如果少写where等各种原因,我们可能会误伤数据库.所以我们可以以安全模式来登录数据库
```
mysql -uroot -S /data/3307/tmp/mysql.sock -U -p
```
设置别名
```
alias mysql='mysql -U'
```
#### -U
Only allow UPDATE and DELETE that uses keys.
