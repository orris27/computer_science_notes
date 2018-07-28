## 1. MySQL
返回程序是minion执行,所以如果要写入MySQL的话,minion必须安装MySQL-python,并且能访问mysql等
+ 减少master负担,提高效率
> https://docs.saltstack.com/en/latest/ref/returners/all/salt.returners.mysql.html
1. 创建表结构
2. minion端安装依赖包
```
sudo yum install MySQL-python -y
```
3. 保证minion端能连接上数据库
4. 在master和minion的配置文件里写入,并重启master和minion
```
mysql.host: '172.19.28.82'
mysql.user: 'salt'
mysql.pass: 'salt'
mysql.db: 'salt'
mysql.port: 3306
```
5. 刷新pillar
```
salt '*' saltutil.refresh_pillar
```
