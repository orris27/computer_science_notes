### 1. 设置root密码
```
sudo mysqladmin -uroot password '123' -S /data/3307/tmp/mysql.sock
```

### 2. 删除不必要的用户
```
drop user ""@centos1;
drop user ""@localhost;
drop user "root"@centos1;
```

### 3. 查询数据库时避免通配符
假设test只有id,name两个属性.那么`select id,name from test`会比使用通配符的`select * from test`效率高.因为后者需要匹配,前者直接插

### 4. 批量插入
```
insert into test values(1,'orris'),(2,'mirai');
```

### 5. 设置权限时采用最小权限
增删改查(insert,delete,update,select)就足够了
