## 备份和还原
逻辑导出sql语句,然后逻辑导出
### mysqldump
#### 常数
1. `-B`: 创建数据库并使用该数据库(相比于不加B参数而言),而且可以同时导出多个数据库
2. `-A`: 备份所有数据库
3. `--compact`:输出更少的信息,调试用
4. `-d`:只备份表结构
5. `-t`:只备份数据
6. `-F`:在导出备份的数据时,切割binlog
7. `--master-data=1`:在输出信息中指定从哪个binlog的哪个位置开始.`=2`的话就是变成注释
8. `-x`:锁所有表
9. `-l`: Lock all tables for read
10. `--single-transaction`: 适合InnoDB事务数据库备份.在备份期间如果有人修改数据库的话,备份过程不会理会
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
备份db_test数据库
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
##### 工具
1. mysqlbinlog 导出/查看增量备份文件(二进制文件)
+ `-d`:指定数据库(不写`-d`就是所有的数据库都扔进去)
+ `--start-position=N`:指定从binlog的哪个位置开始解析成sql语句
+ `--stop-position=N`:指定到binlog的哪个位置结束解析成sql语句
- `mysqlbinlog`有个bug,就是每次使用时都会读取MySQL的配置文件,而charset又被认为是错误的=>要么执行前注释掉,要么加`--no-defaults`参数
```
mysqlbinlog mysqlbin_orris.000001 # 所有数据库都解析成sql语句,并输出到屏幕上
mysqlbinlog --no-defaults mysqlbin_orris.000001
mysqlbinlog -d db_test mysqlbin_orris.000001 > /opt/db_default_bin_bak.sql 
mysqlbinlog -d db_test mysqlbin_orris.000001 > /opt/db_default_bin_bak.sql 
mysqlbinlog -d db_test mysqlbin_orris.000001 > /opt/db_default_bin_bak.sql 
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
