### 创建MySQL多实例
1. 将配置文件,启动程序和数据文件放到不同地方,并进行配置
2. 以各自的目录初始化各自的数据文件 (server_id等一定要不一样)
3. 用各自的配置文件启动mysql服务
4. 使用

#### 步骤
1. 将配置文件,启动程序和数据文件放到不同地方,并进行配置
```
sudo mkdir -p /data/{3306,3307}/data
sudo cp /etc/my.cnf /data/3306/
sudo vim /data/3306/my.cnf
###
:%s/application\/mysql-5.7.22/data\/3306/g
server_id=1
###
sudo cp /data/3306/my.cnf /data/3307
sudo sed -i 's/3306/3307/g' /data/3307/my.cnf 
sudo sed -i 's/server-id=1/server-id=3/g' /data/3307/my.cnf
```
2. 以各自的目录初始化各自的数据文件(basedir不能写原来的安装目录,如`/application/mysql`)
```
sudo mkdir /data/{3306,3307}/{logs,run,tmp}
sudo chown -R mysql.mysql /data/{3306,3307}/{logs,run,tmp}
sudo mysqld --initialize-insecure --user=mysql --basedir=/data/3306 --datadir=/data/3306/data/
sudo mysqld --initialize-insecure --user=mysql --basedir=/data/3307 --datadir=/data/3307/data/
```
3. 用各自的配置文件启动mysql服务就好了
```
sudo chown -R mysql.mysql /data
sudo echo "" | sudo tee /data/{3306,3307}/logs/mysql_error.log
sudo chown -R mysql.mysql /data/{3306,3307}/logs/mysql_error.log
sudo mysqld_safe --defaults-file=/data/3306/my.cnf &
sudo mysqld_safe --defaults-file=/data/3307/my.cnf &
```
4. 使用
```
sudo mysql -uroot -S /data/3306/tmp/mysql.sock
sudo mysql -uroot -S /data/3307/tmp/mysql.sock 
```
5. 停止服务
```
sudo mysqladmin -uroot -S /data/3306/tmp/mysql.sock shutdown -p
sudo mysqladmin -uroot -S /data/3307/tmp/mysql.sock shutdown -p
```

#### 附录
##### 1. 目录结构
```
/data
|-- 3306
|   |-- data
|   |   |-- auto.cnf
|   |   |-- ib_buffer_pool
|   |   |-- ib_logfile0
|   |   |-- ib_logfile1
|   |   |-- ibdata1
|   |   |-- ibtmp1
|   |   |-- mysql [error opening dir]
|   |   |-- performance_schema [error opening dir]
|   |   `-- sys [error opening dir]
|   |-- logs
|   |   |-- mysql_error.log
|   |   `-- mysql_slow_query.log
|   |-- my.cnf
|   |-- run
|   |   `-- mysqld.pid
|   `-- tmp
|       |-- mysql.sock
|       `-- mysql.sock.lock
`-- 3307
    |-- data
    |   |-- auto.cnf
    |   |-- ib_buffer_pool
    |   |-- ib_logfile0
    |   |-- ib_logfile1
    |   |-- ibdata1
    |   |-- ibtmp1
    |   |-- mysql [error opening dir]
    |   |-- performance_schema [error opening dir]
    |   |-- seven [error opening dir]
    |   `-- sys [error opening dir]
    |-- logs
    |   |-- mysql_error.log
    |   `-- mysql_slow_query.log
    |-- my.cnf
    |-- run
    |   `-- mysqld.pid
    `-- tmp
        |-- mysql.sock
        `-- mysql.sock.lock
```
##### 2. 配置文件
###### 1. 3306
```
[client]
port = 3306
socket = /data/3306/tmp/mysql.sock

[mysqld]
port = 3306
socket = /data/3306/tmp/mysql.sock
pid_file = /data/3306/run/mysqld.pid
datadir = /data/3306/data
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

log-error = /data/3306/logs/mysql_error.log


slow_query_log = 1 
slow_query_log_file = /data/3306/logs/mysql_slow_query.log
long_query_time = 2 


tmp_table_size = 128M
max_heap_table_size = 32M
#query_cache_type = 0
query_cache_type = 1
query_cache_size = 256M
query_cache_limit = 2M

server-id=1
```
###### 2. 3307
```
[client]
port = 3307
socket = /data/3307/tmp/mysql.sock

[mysqld]
port = 3307
socket = /data/3307/tmp/mysql.sock
pid_file = /data/3307/run/mysqld.pid
datadir = /data/3307/data
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

log-error = /data/3307/logs/mysql_error.log


slow_query_log = 1 
slow_query_log_file = /data/3307/logs/mysql_slow_query.log
long_query_time = 2 


tmp_table_size = 128M
max_heap_table_size = 32M
#query_cache_type = 0
query_cache_type = 1
query_cache_size = 256M
query_cache_limit = 2M

server-id=3
```

### MySQL多实例脚本
复制下面的文本,作为`/data/3306/mysql`,并赋予可执行权限,使用`/data/3307/mysql [start|stop|restart]`,3307根据自己的端口改就行了
```
#! /bin/sh
PASSWORD="serena2ash"
PORT=3307

source /etc/init.d/functions


function start_mysql () {
	/application/mysql/bin/mysqld_safe --defaults-file=/data/$PORT/my.cnf >/dev/null 2>&1 &
	if [ $? -eq 0 ]
		then 
			action "Mysql starts successfully" /bin/true
			return 0
		else 
			action "MySQL fails to start" /bin/false
			return 1
	fi
}

function stop_mysql(){
	/application/mysql/bin/mysqladmin -uroot -S /data/$PORT/tmp/mysql.sock shutdown -p"$PASSWORD" >/dev/null 2>&1
	if [ $? -eq 0 ]
		then 
			action "Mysql stops successfully" /bin/true
			return 0
		else 
			action "MySQL fails to stop" /bin/false
			return 1
	fi
}


# ensure the number of arguments to be 1
[ $# -ne 1 ] && {
    echo "Usage: /data/$PORT/mysql [start|stop|restart]" 
    exit 1
}

# ensure that the user is the root
[ $UID -ne 0 ] && {
	echo "You must be root"
	exit 2
}

# start
if [ $1 = "start" ]
	then
		# check if the mysql of the pid has been started 
		[ `netstat -lntup | grep -E "${PORT}.*mysqld" | wc -l` -ne 0 ] && {
			echo "Mysql is running"
			exit 4
		}
		start_mysql && exit 0 || exit 1
fi

# stop
if [ $1 = "stop"  ]
	then
		stop_mysql && exit 0 || exit 1
fi

# restart 
if [ $1 = "restart"  ]
	then
		stop_mysql && start_mysql && exit 1 || exit 0
fi

```
#### 问题
1. MySQL启动时输出太多,怎么办
+ 参考`mysql.server`的处理,将`&`后台符号放到`>/dev/null 2>&1`后面就行了
```
/application/mysql/bin/mysqld_safe --defaults-file=/data/$1/my.cnf >/dev/null 2>&1 &
```
2. `netstat -lntup | grep -E '$1.*mysqld' | wc -l`这条语句在shell($1为3307)中输出1的时候,为什么在脚本里执行的时候,会变成0呢
+ 因为我用单引号包住了$1,实际上应该用`"$2.*mysqld"`


### 常见问题
#### 1. 权限不够或找不到文件
##### 解决方法
1. 创建出所有需要的目录
2. 授权所有目录
3. 如果还是权限不够,说明还要创建并授权指定的文件,如`mysql-error.log`.不仅是目录,连文件也要自己创建出来

