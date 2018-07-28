## 0. syntax
1. `:`表示层级关系
## 1. client_acl
### 1-1. 格式
ACL是针对用户的权限访问控制=>用户为模块.方法的上级
#### 1-1-1. 用户,权限
```
client_acl:
  Linux用户名:
    - 模块.方法
```
###### 允许orris使用`test.ping`和network模块下的所有方法
```
client_acl:
  orris:
    - test.ping
    - network.*
```
#### 1-1-1. 用户,主机,权限

```
client_acl:
  Linux用户名:
    - 主机名:
      - 模块.方法
```
###### 允许orris只能在某台主机上使用`test.ping`和network模块下的所有方法
```
#########
client_acl:
  orris:
    - linux-node1*:
      - test.ping
#########
sudo systemctl restart  salt-master
[orris@linux-node1 ~]$ salt 'linux-node3*' network.arp # linux-node1是不行的
```
### 1-0. 问题
#### 1-0-1. `Failed to authenticate! This is most likely because this user is not permitted to execute commands, but there is a small possibility that a disk error occurred (check disk/inode usage).`
有两种可能
+ 层级关系没有写好,如冒号没写
+ salt的target只能选择规定的主机


## 2. client_acl_blacklist
可以配置acl的黑名单形式



