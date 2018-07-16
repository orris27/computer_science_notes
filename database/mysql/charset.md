# 字符集
MySQL的字符集为字符集+校对规则.要保证**MySQL的字符集=库/表的字符集=程序的字符集**
+ 校对规则:比较字符串的规则
+ 字符集:client,connection,database,results,server,system
## 0. 解决方案
1. 配置MySQL的配置文件(多实例似乎也使用`/etc/my.cnf`里的字符集设置)
```
[client]
default-character-set = utf8mb4 # 配置客户端的mysql不用重启mysql服务,服务端就需要了
[server]
character-set-server = utf8mb4
collation-server = utf8mb4_unicode_ci
init_connect='SET NAMES utf8mb4'
```
2. 修改Linux系统的语言
```
sudo vim /etc/sysconfig/i18n # CentOS7貌似不行
```
3. 检测是否修改成功
执行`show variables like 'character_set%';`语句,如果规则里的6条都一样,就大功告成了
## 1. 规则
| way                             | client | connection | database | results | server | system |
| :---                            | :---   |  :----     | :----    | :---    | :---   | :---   | 
| set names                       | 1      | 1          |          |       1 |        |        | 
| --default-character-set         | 1      | 1          |          |       1 |        |        | 
| \[client\]default-character-set | 1      | 1          |          |       1 |        |        | 
| \[mysqld\]character-set-server  |        |            |    1     |         |   1    |        | 
| /etc/sysconfig/i18n             |        |            |          |         |        |   1    | 
## 2. 默认
+ 与自己的编译参数一致,否则就是latin1
+ collation是校对的.(具体我也不太清楚)
```
-DDEFAULT_CHARSET=utf8 \
-DDEFAULT_COLLATION=utf8_general_ci \
```
## 3. utf8mb4
+ utf8mb4表示可以兼容4字节的Unicode编码.mb4表示most bytes 4.
+ utf8mb4是utf8的超集,支持emoji等,兼容utf8
+ 推荐使用utf8mb4,虽然浪费一定空间,但值得
+ 移动端
## 4. 支持的字符集
在编译时指定的`DEXTRA_CHARSETS`参数中就是支持的字符集
```
-DEXTRA_CHARSETS=gbk,gb2312,utf8,ascii \
```
## 5. 指定字符集创建数据库
```
show create database db_default;
show create database db_default\G # 我在编译时指定了utf8编码和校对,所以这里也是utf8
# Create Database: CREATE DATABASE `db_default` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci */

create database db_gbk default character set gbk collate gbk_chinese_ci;
create database db_utf8 default character set utf8 collate utf8_general_ci;
# Create Database: CREATE DATABASE `db_utf8` /*!40100 DEFAULT CHARACTER SET utf8 */
```
## 6. 乱码解决
1. DML,DDL的时候都要更改当前客户端的字符集,保证和服务端的字符集相同
2. 调整字符集的根本目的是**MySQL的字符集=库/表的字符集=程序的字符集**
3. 如果都6条都相同而还是乱码,可以查看下库和表的字符集是否正确
## 7. 查看校对
```
show character set;
```
## 8. 设置字符集
### 1. 设置当前mysql客户端的字符集
```
set names latin1; 
```
### 2. 设置导入sql语句时客户端的字符集
```
mysql -uroot -S /data/3307/tmp/mysql.sock -p --default-character-set=latin1 test< ch.sql
```
