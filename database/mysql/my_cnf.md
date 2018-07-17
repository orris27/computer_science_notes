## MySQL 5.7.22
+ socket文件:`/application/mysql-5.7.22/tmp/mysql.sock`
+ pid文件:`/application/mysql-5.7.22/run/mysqld.pid`
+ 数据目录:`/application/mysql-5.7.22/data`
+ 日志目录:`/application/mysql-5.7.22/logs`
### 注意
1. 如果要配置`log-bin`的话,一定要写上`server-id`
### my.cnf
```
[client]
port = 3306
socket = /application/mysql-5.7.22/tmp/mysql.sock
default-character-set = utf8mb4

[mysqld]
server-id=1

#auto_increment_increment = 2
#auto_increment_offset = 2 # 2,4,6,...的方式递增,用于master-master的双向同步架构


port = 3306
socket = /application/mysql-5.7.22/tmp/mysql.sock
pid_file = /application/mysql-5.7.22/run/mysqld.pidid
datadir = /application/mysql-5.7.22/data
default_storage_engine = InnoDB
max_allowed_packet = 512M
max_connections = 2048
open_files_limit = 65535

skip-name-resolve
lower_case_table_names=1

character-set-server = utf8mb4
collation-server = utf8mb4_unicode_ci
init_connect='SET NAMES utf8mb4'


# 下面的配置好像是和thread有关,我的服务器为1核,所以可能不能用
#innodb_additional_mem_pool_size = 4M
#innodb_data_home_dir =
#innodb_file_io_threads = 4
#innodb_force_recovery=1
#innodb_fast_shutdown
#innodb_log_group_home_dir
#innodb_flush_method=fdatasync
#innodb_data_file_path = ibdata1:128M:autoextend
#innodb_thread_concurrency = 8

innodb_flush_log_at_trx_commit = 2
innodb_log_buffer_size = 16M
innodb_log_files_in_gr
..		
charset.md	Update charset.md	a day ago
installation.mdoup = 3
innodb_max_dirty_pages_pct = 90
innodb_lock_wait_timeout = 120
innodb_buffer_pool_size = 32M
innodb_log_file_size = 4M 
innodb_file_per_table = 0 


key_buffer_size = 256M

log-error = /application/mysql-5.7.22/logs/mysql_error.log
skip-name-resolve
slave-skip-errors = 1032, 1062, 1007


log-bin = /application/mysql-5.7.22/logs/mysql_bin
log-bin-index = /application/mysql-5.7.22/logs/mysql_bin.index
binlog_cache_size = 1M
max_binlog_cache_size = 1M
max_binlog_size = 2M
#log-slave-updates # 如果slave也要做master的话,log-slave-updates和log-bin都要打开
expire_logs_days = 5 # log-bin的过期时间设置为5天
#binlog_format = "STATEMENT"
#binlog_format = "ROW"
binlog_format = "MIXED" # 官方推荐

#replicate-do-db = mysql # 需要slave复制的数据库
replicate-ignore-db = mysql # 不需要slave复制的数据库(写在master/slave好像都可以)

#binlog-do-db =  # log-bin记录该数据库的日志
binlog-ignore-db = mysql # log-bin不要记录该数据库的日志
binlog-ignore-db = performance_schema
binlog-ignore-db = information_schema

#relay-log = /application/mysql-5.7.22/logs/relay_bin
#relay-log-info-file = /application/mysql-5.7.22/logs/relay_log.info

slow_query_log = 1 # 打开慢查询日志功能
slow_query_log_file = /application/mysql-5.7.22/logs/mysql_slow_query.log
long_query_time = 2 # 超过2秒的查询记录下来


tmp_table_size = 128M
max_heap_table_size = 32M
#query_cache_type = 0
query_cache_type = 1
query_cache_size = 256M
query_cache_limit = 2M

[mysqldump]
quick 
max_allowed_packet = 2M
```
