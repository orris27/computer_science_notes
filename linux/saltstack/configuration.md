## 0. 语法
1. `:`表示层级关系
2. `-`:表示列表

## 1. 文件
1. top.sls
+ 一定放在base环境下
+ `top.sls`是top file,因为master配置文件里写着`state_top: top.sls`
```
环境:
  匹配minion:
    - 加载什么模块
```
###### 例子
```
base:
  'web:hello':
    - match: grain
    - apache
```
## 2. 普通状态
### 2-1. 文件管理
```
minion端的文件名:
  file.managed:
    - source: 相对于在哪个环境,就在哪个环境里找,具体的文件名
    - 用户
    - 组
    - 权限
    
```
#### ### 2-2. 配置
2-2-1. 在master配置文件里设置top file的目录
```
sudo vim /etc/salt/master
##############
file_roots:
  base:
    - /srv/salt/base
  test:
    - /srv/salt/test
  prod:
    - /srv/salt/prod
##############
sudo systemctl restart salt-master
sudo mkdir /srv/salt/{base,test,prod}
```
#### 2-2-2. 写基本的文件管理
```
sudo mkdir /srv/salt/base/files
cd /srv/salt/base/
sudo vim dns.sls
###############
/etc/resolv.conf:
  file.managed:
    - source: salt://files/resolv.conf
    - user: root
    - group: root
    - mode: 644
###############
sudo mv /etc/resolv.conf /srv/salt/base/files/
```
#### 2-2-3. 执行文件状态
##### 2-2-3-1. 直接执行
```
sudo salt '*' state.sls dns
```

##### 2-2-3-2. 高级状态,每次执行状态都会执行
从top file开始读,top file怎么指定状态就执行
1. 修改base环境下的`top.sls`文件
```
base:
  '*':
    - dns
```
2. 执行高级状态
```
sudo salt '*' state.highstate
```

## 3. 应用
1. 系统初始化
2. 功能模块
3. 业务模块


