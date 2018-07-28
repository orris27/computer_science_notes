## 1. MySQL
返回程序是minion执行,所以如果要写入MySQL的话,minion必须安装MySQL-python,并且能访问mysql等
+ 减少master负担,提高效率
> https://docs.saltstack.com/en/latest/ref/returners/all/salt.returners.mysql.html
1. 创建表结构
```
CREATE DATABASE  `salt`
  DEFAULT CHARACTER SET utf8
  DEFAULT COLLATE utf8_general_ci;

USE `salt`;

--
-- Table structure for table `jids`
--

DROP TABLE IF EXISTS `jids`;
CREATE TABLE `jids` (
  `jid` varchar(255) NOT NULL,
  `load` mediumtext NOT NULL,
  UNIQUE KEY `jid` (`jid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
CREATE INDEX jid ON jids(jid) USING BTREE;

--
-- Table structure for table `salt_returns`
--

DROP TABLE IF EXISTS `salt_returns`;
CREATE TABLE `salt_returns` (
  `fun` varchar(50) NOT NULL,
  `jid` varchar(255) NOT NULL,
  `return` mediumtext NOT NULL,
  `id` varchar(255) NOT NULL,
  `success` varchar(10) NOT NULL,
  `full_ret` mediumtext NOT NULL,
  `alter_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  KEY `id` (`id`),
  KEY `jid` (`jid`),
  KEY `fun` (`fun`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Table structure for table `salt_events`
--

DROP TABLE IF EXISTS `salt_events`;
CREATE TABLE `salt_events` (
`id` BIGINT NOT NULL AUTO_INCREMENT,
`tag` varchar(255) NOT NULL,
`data` mediumtext NOT NULL,
`alter_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
`master_id` varchar(255) NOT NULL,
PRIMARY KEY (`id`),
KEY `tag` (`tag`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
```
2. minion端安装依赖包
```
sudo yum install MySQL-python -y
```
3. 保证minion端能连接上数据库
```
grant all on salt.* to 'salt'@'172.19.28.%' identified by 'salt';
```
4. 在minion的配置文件里写入,并minion
```
sudo vim /etc/salt/minion
######################
#return: mysql
mysql.host: '172.19.28.82'
mysql.user: 'salt'
mysql.pass: 'salt'
mysql.db: 'salt'
mysql.port: 3306
######################
sudo systemctl restart salt-minion
```
(5. 刷新pillar)(不需要)
```
salt '*' saltutil.refresh_pillar
```
6. 测试
```
sudo salt '*' test.ping --return mysql
mysql -h172.19.28.82 -usalt -psalt
#######
use salt
select * from salt_returns\G
#######
```
