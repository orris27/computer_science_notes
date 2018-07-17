## 1. 备份和还原
逻辑导出sql语句,然后逻辑导出
### mysqldump
#### 常数
1. `-B`: 创建数据库并使用该数据库(相比于不加B参数而言),而且可以同时导出多个数据库
2. `-A`: 备份所有数据库
3. `--compact`:输出更少的信息,调试用
4. `-d`:只备份表结构
5. `-t`:只备份数据
6. `-F`:在导出备份的数据时,切割binlog
7. `--master-data=1`:在输出信息中指定从哪个binlog的哪个位置开始.slave就可以不用添加MASTER_LOG_FILE和MASTER_LOG_POS的信息了.`=2`的话就是变成注释
8. `-x`:锁所有表
9. `-l`: Lock all tables for read
10. `--single-transaction`: 适合InnoDB事务数据库备份.在备份期间如果有人修改数据库的话,备份过程不会理会.就是ACID中的I(隔离性)
##### 生产环境推荐
###### 常规备份
`--master-data`选1或2根据自己习惯.下面给出的是整理所有的数据库
1. MyISAM
```
mysqldump -uroot -p'pwd' -A -B --master-data=2 --events -x | gzip >/opt/all.sql.gz
```
2. InnoDB
```
mysqldump -uroot -p'pwd' -A -B --master-data=2 --events --single-transaction | gzip >/opt/all.sql.gz
```
###### 更详细点的备份
1. MyISAM
```
mysqldump --user=root --all-databases --flush-privileges --lock-all-tables \
--master-data=1 --flush-logs --triggers --routines --events \
--hex-blob > $BACKUP_DIR/full_dump_$BACKUP_TIMESTAMP.sql
```
2.InnoDB
```
mysqldump --user=root --all-databases --flush-privileges --single-transaction \
--master-data=1 --flush-logs --triggers --routines --events \
--hex-blob > $BACKUP_DIR/full_dump_$BACKUP_TIMESTAMP.sql
```
#### 备份
备份db_test数#log-bin = /usr/local/mysql/binlogs/mysql-bin
relay-log = /usr/loal/mysql/logs/relay-bin
relay-log-info-file = /usr/loal/mysql/logs/relay-log.info
据库
```
# mysqldump -uroot -S /data/3307/tmp/mysql.sock -p db_test
# mysqldump -uroot -S /data/3307/tmp/mysql.sock -p db_test table_test1 table_test2 # 不加-B的话,可以只备份一个表(前面是库,后面都是表)(从create table开始备份)
# mysqldump -uroot -S /data/3307/tmp/mysql.sock -B -p db_test
mysqldump -uroot -S /data/3307/tmp/mysql.sock -B -p db_test > /opt/db_test.sql 
# mysqldump -uroot -S /data/3307/tmp/mysql.sock -B -p db_test | gzip > /opt/db_test.sql.gz # 推荐
# mysqldump -uroot -S /data/3307/tmp/mysql.sock -B -p db_test db_test2 | gzip > /opt/db_test.sql.gz # 推荐
```
#### 分库备份
将不同的库备份到各自对应的文件,这样以后可以方便局部还原
1. 
```
mkdir /opt/bak
mysql -uroot -S /data/3307/tmp/mysql.sock -p'pwd' -e 'show databases;' | egrep -vi 'Database|infor|perfor' | sed -r 's#^(.*)#mysqldump -uroot -p"pwd" -S /data/3307/tmp/mysql.sock -B \1 | gzip > /opt/bak/\1.sql.gz#g' | bash
```
2. 
```
#! /bin/sh

for dbname in `/application/mysql/bin/mysql -uroot -S /data/3307/tmp/mysql.sock -p'pwd' -e 'show databases;' | egrep -vi 'Database|infor|perfor'`
do
	/application/mysql/bin/mysqldump -uroot -S /data/3307/tmp/mysql.sock -B -p'pwd' ${dbname} | gzip > /opt/bak/${dbname}.sql.gz
done
```
#### 还原(-B)
使用`-B`导出的sql文件会自动创建数据库并使用数据库,因此导出时不用指定数据库
1. 没有压缩
```
mysql -uroot -S /data/3307/tmp/mysql.sock -p < /opt/db_test.sql 
```
2.压缩
```
gzip -d db_test.sql.gz
mysql -uroot -S /data/3307/tmp/mysql.sock -p < /opt/db_test.sql # 推荐
```
#### 查看
```
egrep -v '#|\/|^$|--' /opt/db_test.sql
```

#### 增量备份
实时备份
1. 我们不能实时逻辑备份,但我们可以让MySQL自己实时记录我们的每一个更改操作
2. 数据库需要恢复一般是某个员工的一个错误操作
3. 在我们逻辑备份后的一段时间内,如果某个瓜皮搞事的话,我们就要恢复数据
4. 通过MySQL记录的增量备份,我们可以看到这段时间内的所有语句.先逻辑备份后增量备份就好了
5. 但是我们不能保证这一期间没有其他操作.比如逻辑备份期间如果也会有记录,增量备份中不能有它.要么停止,要么换个位置,要么我们提前拿出来
##### mysqlbinlog
导出/查看增量备份文件(二进制文件)
+ `-d`:指定数据库(不写`-d`就是所有的数据库都扔进去)
+ `--start-position=N`:指定从binlog的哪个位置开始解析成sql语句
+ `--no-defaults`:不读取配置文件,直接执行
+ `--base64-output="DECODE-ROWS" -v`:两个参数配合起来,可以解析ROW模式下的log-bin文件
+ `--stop-position=N`:指定到binlog的哪个位置结束解析成sql语句
- `mysqlbinlog`有个bug,就是每次使用时都会读取MySQL的配置文件,而charset又被认为是错误的=>要么执行前注释掉,要么加`--no-defaults`参数
```
mysqlbinlog mysqlbin_orris.000001 # 所有数据库都解析成sql语句,并输出到屏幕上
mysqlbinlog --no-defaults mysqlbin_orris.000001
mysqlbinlog -d db_test mysqlbin_orris.000001 > /opt/db_default_bin_bak.sql 
mysqlbinlog -d db_test mysqlbin_orris.000001 > /opt/db_default_bin_bak.sql 
mysqlbinlog -d db_test mysqlbin_orris.000001 > /opt/db_default_bin_bak.sql 
```
##### log-bin参数
###### 模式
1. Statement Level
+ 日志小
+ 难复制一些新的函数
2. Row Level
+ 按照行来记录语句.如一条update可能分成很多语句
+ 可以解决Statement Level一些难以解决的问题
+ 日志很大
3. Mixed
+ Statement Level+Row Level
+ 根据语句判断什么时候使用Statement Level,什么时候使用Row Level
###### 更改模式
1. `set global binlog_format="STATEMENT";`
2. 
```
#binlog_format = "STATEMENT"
binlog_format = "ROW"
#binlog_format = "MIXED"
```

##### 实战
###### 1. 1个MySQL,没有主从,允许停机,只恢复db_test
定时备份,然后增量.发现问题时,停止写入.根据全量备份的`--master-data=2`的参数获得binlog起点,刷新binlog得到终点.删除binlog中的错误操作.全量+增量
0. 假定MySQL已经开启log-bin
1. 先备份数据库db_test
2. 各种操作+删除了db_test
3. 停止写入数据库(防火墙或者锁表)
4. 获取log-bin的起点和终点
5. 在增量备份中去掉之前那个骚操作
6. 逻辑备份+增量备份

+ 定时的全量备份
```
mysqldump -uroot -B db_test -S /data/3307/tmp/mysql.sock -p --master-data=2 --single-transaction | gzip >/opt/db_test.sql.gz
```
+ 一些操作
```
mysql -uroot -p -S /data/3307/tmp/mysql.sock
###
create table db_test;
use db_test;
create table table_test(id int);
insert into table_test values(1);
drop database db_test;
exit;
###
```
+ 发现问题,只能暂停写入了
```
mysql -uroot -p -S /data/3307/tmp/mysql.sock
###
flush table with read lock;
###
```
+ 获得增量备份的起点和终点
```
mysqladmin -S /data/3307/tmp/mysql.sock -uroot -p flush-log
gzip -d db_test.sql.gz
grep -i 'change' db_test.sql
# -- CHANGE MASTER TO MASTER_LOG_FILE='mysql_bin.000001', MASTER_LOG_POS=2871;
```
+ 获取增量备份
```
mysqlbinlog --no-defaults --start-position=2871 -d db_test /data/3307/logs/mysql_bin.000001 > /opt/db_test_bin.sql
```
+ 删除误操作
```
sudo vim /opt/db_test_bin.sql
# 将drop 操作下面的内容删除
#drop database student
#/*!*/;
#SET @@SESSION.GTID_NEXT= 'AUTOMATIC' /* added by mysqlbinlog */ /*!*/;
#DELIMITER ;
#/*!50003 SET COMPLETION_TYPE=@OLD_COMPLETION_TYPE*/;
#/*!50530 SET @@SESSION.PSEUDO_SLAVE_MODE=0*/;
```
+ 解锁
```
mysql -uroot -p -S /data/3307/tmp/mysql.sock
###
unlock tables;
###
```
+ 用全量+增量恢复
```
mysql -uroot -S /data/3307/tmp/mysql.sock -p < /opt/db_test.sql
mysql -uroot -S /data/3307/tmp/mysql.sock  db_test -p</opt/db_test_bin.sql
逻辑导出sql语句,然后逻辑导出
```
## 2. 主从同步
实际上是备份方案,主MySQL的数据会放到从MySQL的数据里

### 主MySQL突然宕机怎么办
1. 切换到slave(分发hosts)
=>master和slave基本一致但还是有不一致的地方,怎么办?

#### 解决方法
1. binlog
2. 双写
3. Google开发了一个半同步的工具,就是只有master和slave一致才写入
+ 指定一个slave,进行半同步.加入宕机了,就将该slave变成master
#### slave=>master
1. 根据master.info,从所有从库中找到同步最好(POS最大)的slave
2. 确保所有relay-log全部更新完毕
+ 每个slave上执行stop slave io_thread;show processlist;
+ 直到看到Has read all relay log;表示slave更新都执行完毕
3. 对于选定的slave,执行`stop slave;restart master;quit;`
4. 进入到数据库数据目录,删除`master.info`,`relay-log.info`
```
cd xxx/data
rm -f master.info relay-log.info
```
5. 配置新的master
+ 开启log-bin
+ 删除`log-slave-updates`,readonly等参数
+ 检查授权表
6. 更新其他slave
```
stop slave;
change master to master_host='172.19.28.xx';
start slave;
show slave status;
```


### 实现
1. 配置文件
+ master打开log-bin
```
log-bin = /application/mysql-5.7.22/logs/mysql_bin
log-bin-index = /application/mysql-5.7.22/logs/mysql_bin.index
```
+ slave打开relay-log
```
relay-log = /usr/loal/mysql/logs/relay-bin
relay-log-info-file = /usr/loal/mysql/logs/relay-log.info
```
+ slave关闭log-bin
```
#log-bin = /usr/local/mysql/binlogs/mysql-bin
```
+ master和slave的server-id设置成不一样(ip等区分)

2. master建立从库复制的账号
```
grant replication slave on *.* to 'rep'@' 172.19.28.%' identified by 'rep123';
flush privileges;tongbu
```

3. 为master加锁(master不要退出MySQL的客户端,直到解锁为止)
```
flush table with read lock;
```

4. 拿到点
```
show master status;
show master logs;
```

4. 逻辑导出所有数据库的SQL语句(重开命令行窗口)
```
mysqldump -uroot -p'pwd' -S /data/3306/mysql.sock -A -B --events | gzip >/opt/rep.sql.gz
```

5. 再拿到点,检查点是否变化
+ 如果和上面一致,说明备份完整了
```
show master status;
```

6. 给master打开锁
```
unlock tables;
```

7. 将导出的SQL语句传给slave服务器
```
rsync -avz -e 'ssh' /opt/rep.sql.gz orris@172.19.28.83:/home/orris/sql/
```

7. slave处导入SQL语句
```
gzip -d /home/orris/sql/rep.sql.gz
mysql -uroot -p'pwd' < /home/orris/sql/rep.sql
```

8. slave处在MySQL的客户端里配置master的信息(信息会记录在对应MySQL实例的数据目录下的`master.info`里)
```
mysql -uroot -p'pwd'
###
CHANGE MASTER TO
MASTER_HOST='172.19.28.82',
MASTER_PORT=3306,
MASTER_USER='rep',
MASTER_PASSWORD='rep123',
MASTER_LOG_FILE='mysql_bin.00000x', #如果--master-data=1的话,就不用写这句和下句了.根据上面show master status填
MASTER_LOG_POS=xx; #根据上面show master status填
```

9. slave打开开关
```
# MySQL client:
start slave;
```

10. 检查slave是否正常工作
```
# MySQL client:
show slave status\G
####
Slave_IO_Running: Yes
Slave_SQL_Runing: Yes
Seconds_Behind_Master: 0 # 落后master的秒数
#### 
```

### 错误
1. `File '/usr/loal/mysql/logs/relay-bin.index' not found (Errcode: 2 - No such file or directory)`
=> 配置文件错误.应该是`/usr/local`
2. 在锁表期间仍能更改数据?
+ 过了`wait_timeout`和`interactive_timeout`时间后,即使锁表,也可以修改了
+ `wait_timeout`和`interactive_timeout`不能太小,不然备份有问题

### 原理
类似于搬家工人一样,我们(master)将行李放到行李房(binlog),然后搬家工人(slave的IO)发现有有新的行李,这个新的行李上有指定的房间和位置(binlog的文件和位置),就带到目的地(根据master-info将master的log-bin添加到slave的relay-log中),目的地有另一个负责人,她发现多了行李,就会根据上面的信息搬到指定的位置. master确认slave的信息是自己的主进程来完成的.如果slave也要做master的话,那么就要设置`log-bin`和`log-slave-updates`
+ `master-info`
	- 记录master信息的文件
	- 默认放在数据目录下
	- 内容=我们在MySQL客户端里的change master
	- 记录的是对应master的log-bin的文件名和偏移量
	- 同步一次=>IO进程就会更新master的log-bin的文件名和偏移量
+ `relay-log`
	- 在slave配置文件里的参数
	- 我们一般设置成`relay_bin`,因为它其实=master的`log-bin`
	- IO负责写入
	- SQL会查看
	- SQL会执行
+ `relay-log-info-file`
	- 记录`relay-log`的文件名和位置
	- 记录master的log-bin的文件名和位置
### 主从不同步
如果slave已经有test这个数据库了,那么master创建test这个数据库的话,就会导致数据库不一致
#### 检测状态
`show slave status\G`看是否2个yes和1个0
#### 解决思路
忽略
#### 解决方法
<1> 在配置文件里声明忽略哪些错误
```
# slave's my.cnf
slave-skip-errors = 1032, 1062, 1007
```
<2> 手动将log-bin的位置向下偏移(错误代码后面的内容也会同步过来)
```
# slave MySQL:
stop slave;
set global sql_slave_skip_counter=1;
start slave;
```
### master-master双向同步

1. 保证两个master的表都会主键自动递增(设置`auto_increment_offset``auto_increment_increment`)
2. 互为master/slave,主从一样配置即可.注意写`log-slave-updates`

--------
## 问题
#### 1. 如果我锁表了,那么其他人如果执行修改语句,这条语句会处于等待状态,等我解除锁表后,是不是她的语句就能执行了?
#### 2. 如果master换了个log-bin文件,slave能知道吗?
#### 3.如果我在master这里创建了一个用户,那么slave里会有这个用户吗?
会的.本质:用户<=>mysql.user.同步mysql这个数据库=>同步用户.
##### 不同步所有数据库
在master的参数里说明哪些数据库不同步
```
replicate-ignore-db = mysql # 这一句才真正不会被slave给同步
binlog-ignore-db=mysql
binlog-ignore-db=performance_schema
binlog-ignore-db=information_schema
```
#### 4. 如果master重启MySQL,那么slave还能检测到吗?
还能正常工作
#### 5. 如果将一个表的name属性全变成orris的话,我发现增量备份里面没有这个语句.不知道为什么
因为增量模式设置成ROW了.所以会把update给拆分成很多行级语句.直接用`mysqlbinlog`的话是会乱码的.
##### 解决方法
使用`mysqlbinlog --base64-output=DECODE-ROWS -v`可以查看ROW模式下的bin
```
sudo mysqlbinlog --no-defaults --base64-output="DECODE-ROWS" -v /data/3307/logs/mysql_bin.000004
```
`

