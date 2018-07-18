## MySQL 5.7.22
+ socket文件:`/application/mysql-5.7.22/tmp/mysql.sock`
+ pid文件:`/application/mysql-5.7.22/run/mysqld.pid`
+ 数据目录:`/application/mysql-5.7.22/data`
+ 日志目录:`/application/mysql-5.7.22/logs`
### 注意
1. 如果要配置`log-bin`的话,一定要写上`server-id`
2. 官方注释看这个
> https://github.com/orris27/orris/blob/master/database/mysql/my-innodb-heavy-4G.cnf
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
#autocommit = ON # 自动提交每一句SQL语句.如果OFF了,只有commit执行后才会写入磁盘,否则一直在内存.一旦退出会话,再进去就会消失

port = 3306
socket = /application/mysql-5.7.22/tmp/mysql.sock
pid_file = /application/mysql-5.7.22/run/mysqld.pidid
datadir = /application/mysql-5.7.22/data
default_storage_engine = InnoDB
max_allowed_packet = 512M
max_connections = 2048
open_files_limit = 65535 # 打开文件的数量,可以调大点

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
#innodb_data_file_path = ibdata1:128M:autoextend # InnoDB的数据文件,一开始初始化是128M,以后自动扩展
#innodb_thread_concurrency = 8

# Set this option if you would like the InnoDB tablespace files to be
# stored in another location. By default this is the MySQL datadir.
#innodb_data_home_dir = <directory>

# If set to 1, InnoDB will flush (fsync) the transaction logs to the
# disk at each commit, which offers full ACID behavior. If you are
# willing to compromise this safety, and you are running small
# transactions, you may set this to 0 or 2 to reduce disk I/O to the
# logs. Value 0 means that the log is only written to the log file and
# the log file flushed to disk approximately once per second. Value 2
# means the log is written to the log file at each commit, but the log
# file is only flushed to disk approximately once per second.
innodb_flush_log_at_trx_commit = 2 # 设置成1的话,严格符合ACID.0的话性能最好,安全最差.一般设置成1就好了

innodb_log_buffer_size = 16M
innodb_log_files_in_group = 3
innodb_max_dirty_pages_pct = 90
innodb_lock_wait_timeout = 120
# InnoDB最重要的参数.设置InnoDB存储索引和数据的缓冲区大小.
# 官方建议设置为内存的50%-80%,但是一般最高还是设置成2G会好.
# show variables like 'innodb_buffer%';可以查看buffer_pool的情况
# data/ib_buffer_pool就是我们设置的buffer_pool的大小
# show status like 'innodb_buffer_pool%';可以查看buffer_pool_size的状态
innodb_buffer_pool_size = 2G
innodb_log_file_size = 4M 
innodb_file_per_table = 0 


key_buffer_size = 256M # MyISAM的重要参数,指定MyISAM用来缓存索引的大小

log-error = /application/mysql-5.7.22/logs/mysql_error.log
skip-name-resolve # 如果不加这个参数,可能会导致权限错误
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
#relay-log-info-file = /application/mysql-5.7.22/logs/relay_bin.info

slow_query_log = 1 # 打开慢查询日志功能
slow_query_log_file = /application/mysql-5.7.22/logs/mysql_slow_query.log
long_query_time = 2 # 超过2秒的查询记录下来


tmp_table_size = 256M
max_heap_table_size = 256M
#query_cache_type = 0
query_cache_type = 1
query_cache_size = 64M # MySQL的缓存不是特别好,我们宁可选择memcache做缓存,所以这里就小点好
query_cache_limit = 4M
query_cache_min_res_unit = 2K

read_buffer_size = 1M
sort_buffer_size = 2M
join_buffer_size = 2M # 这些都是针对一个线程来说的,所以调小点就行了

[mysqldump]
quick 
max_allowed_packet = 2M
```
