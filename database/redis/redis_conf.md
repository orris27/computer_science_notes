## 1. 配置redis
1. pid文件:`pidfile /var/run/redis.pid`
2. 日志
    1. 日志级别:`loglevel warning`
    2. 日志文件:`logfile "/var/log/redis.log"`
3. 允许后台启动:`daemonize yes`
4. 绑定IP:`bind 10.0.0.7`
5. 数据文件:`/var/lib/redis_6379`
    + 默认是`./`,这会导致删除redis后数据也删除
    + 这里设置也方便多实例
6. 权限:`requirepass <redis_password>`
    

### 1-1. 主从同步
1. master端要bind自己ip
1. 在slave端设置好`slaveof`,`masterauth`就可以了!(slave不能写入)
```
slaveof 172.19.28.82 6379
# masterauth <master-password> # 如果有密码的话
```

## 2. `redis.conf`解释
```
# redis像Nginx一样可以包含其他配置文件
# include /path/to/local.conf

# 如果设置成127.0.0.1的话,就不能通过内网ip来连接redis
bind 127.0.0.1
protected-mode yes
port 6379
tcp-backlog 511 # 系统的/proc/sys/net/core/somaxconn必须大于这里设置的tcp-backlog,比如说512
timeout 0
tcp-keepalive 300
# 如果yes的话,就会自动守护进程的方式开启
daemonize no
supervised no

# pid位置,需要创建run目录
pidfile /var/run/redis.pid

# 日志的等级一般不要设置成notice,设置成warning
loglevel warning
logfile /var/log/redis.pid

# 默认local0就可以了
# syslog-facility local0

databases 16
always-show-logo yes

# 保存条件
save 900 1
save 300 10
save 60 10000


stop-writes-on-bgsave-error yes
rdbcompression yes
rdbchecksum yes
dbfilename dump.rdb
dir ./

# 在出现错误的时候,是不是要停止保存
slave-serve-stale-data yes
slave-read-only yes

# 从库指定这两行就可以了
# slaveof <masterip> <masterport>
# masterauth <master-password>

repl-diskless-sync no
repl-diskless-sync-delay 5
repl-disable-tcp-nodelay no
slave-priority 100
lazyfree-lazy-eviction no
lazyfree-lazy-expire no
lazyfree-lazy-server-del no
slave-lazy-flush no
appendonly no
appendfilename "appendonly.aof"
appendfsync everysec
no-appendfsync-on-rewrite no
auto-aof-rewrite-percentage 100
auto-aof-rewrite-min-size 64mb
aof-load-truncated yes
aof-use-rdb-preamble no
lua-time-limit 5000
slowlog-log-slower-than 10000
slowlog-max-len 128
latency-monitor-threshold 0
notify-keyspace-events ""
hash-max-ziplist-entries 512
hash-max-ziplist-value 64
list-max-ziplist-size -2
list-compress-depth 0
set-max-intset-entries 512
zset-max-ziplist-entries 128
zset-max-ziplist-value 64
hll-sparse-max-bytes 3000
activerehashing yes
client-output-buffer-limit normal 0 0 0
client-output-buffer-limit slave 256mb 64mb 60
client-output-buffer-limit pubsub 32mb 8mb 60
hz 10
aof-rewrite-incremental-fsync yes
```
