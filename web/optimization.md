## 1. 网站打开慢
### 1-1. 思路
#### 1-1-1. 查找原因
1. 在MySQL中使用`show full processlist`
2. `uptime`
3. `top`+`strace -p 14094`
#### 1-1-2. 根据原因解决问题
### 1-2. MySQL的`like '%杜冷丁%'`这样的语句太多
#### 1-2-1. 分析
1. MySQL中`like '%xx%'`的语句不能使用索引,难优化.而`like 'xx%'`倒是可以使用索引
2. `like '%xx%'`语句多是因为网站有一个搜索框,只要搜索xx,就会使用该语句
#### 1-2-2. 解决
1. 只有登录才能进行搜索(短期)
2. 通过日志查看是不是有人爬站,通过IP干掉她(短期)
3. 在前端设置memcached服务器(长期)
4. 数据库读写分离(长期)
5. 使用Sphinx搜索服务实现搜索
6. 利用c,Ruby开发程序,实现每日读库计算搜索索引

## 2. 会话保持
1. 将会话集中存储到一个地方=>memcache{通知>广播}
+ 会话的存储地方是一个参数=>配置文件里配置会话的存储(比如PHP){I}
> https://github.com/orris27/orris/blob/master/php/optimization.md
2. 互相推送会话{通知>交谈}
+ 软件层可以做session复制
+ tomcat,resin,couchbase
3. 从数据库中更新到每个服务器相应的会话信息{通知>传单}
4. 使用cookies(效率优于memcache,大网站一般都是用cookies保持会话)

## 3. 雪崩效应
前端缓存允许大并发,但是后端数据库只允许小并发.加入这时缓存的大并发突然来袭,后台数据库就凉了
### 解决
1. Nginx/Web服务停止对外(防火墙,停机等)
2. 为了应对大并发的请求,先数据库预热将数据放到缓存服务器上,让请求到缓存并命中
3. 慢慢提起数据库

## 4. 检测文件是否被篡改

> 如何检测文件是否被篡改
> https://coggle.it/diagram/WyEPLdpZzYyocNzH/t/%E8%BF%90%E7%BB%B4

## 5. Web服务器的磁盘满了,但是删除了日志后,磁盘还是满的,怎么办
> https://coggle.it/diagram/WzL2LBN_z0F1AqoH/t/linux%E6%96%87%E4%BB%B6
