## 1. 引入模块
```
  import redis
```
## 2. 连接
```
try:
  r=redis.StrictRedis(host='localhost',port=6379)
except Exception,e:
  print e.message
```
## 3. 操作
### 3-1. 根据数据类型的不同，调用相应的方法，完成读写
更多方法同前面学的命令
```
r.set('name','hello')
r.get('name')
```
### 3-2. pipline
缓冲多条命令，然后一次性执行，减少服务器-客户端之间TCP数据库包，从而提高效率
```
pipe = r.pipeline()
pipe.set('name', 'world')
pipe.get('name')
pipe.execute()
```
