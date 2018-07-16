### MySQL 5.7.22
+ socket文件:`/application/mysql-5.7.22/tmp/mysql.sock`
+ pid文件:`/application/mysql-5.7.22/run/mysqld.pid`
+ 数据目录:`/application/mysql-5.7.22/data`
+ 日志目录:`/application/mysql-5.7.22/logs`
```
[client]
port = 3306
socket = /application/mysql-5.7.22/tmp/mysql.sock
default-character-set = utf8mb4

[mysqld]
port = 3306
socket = /application/mysql-5.7.22/tmp/mysql.sock
pid_file = /application/mysql-5.7.22/run/mysqld.pid
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


innodb_buffer_pool_size = 1024M
innodb_log_file_size = 2048M
innodb_file_per_table = 1

#innodb_flush_log_at_trx_commit = 0


key_buffer_size = 256M

log-error = /application/mysql-5.7.22/logs/mysql_error.log


slow_query_log = 1 # 打开慢查询日志功能
slow_query_log_file = /application/mysql-5.7.22/logs/mysql_slow_query.log
long_query_time = 2 # 超过2秒的查询记录下来


tmp_table_size = 128M
max_heap_table_size = 32M
#query_cache_type = 0
query_cache_type = 1
query_cache_size = 256M
query_cache_limit = 2M

#server-id=1
```
