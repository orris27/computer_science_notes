## 1. 设置pillar
`man pillar`中的Setting Up Pillar下面就介绍了如何设置pillar
1. master配置文件,并重启
+ top.sls一般就放在base环境下
+ 关闭pillar_opts: False
    - 防止获得master的pillar.items
```
sudo vim /etc/salt/master
##############
pillar_roots:
  base:
    - /srv/pillar

pillar_opts: False
##############
sudo systemctl restart salt-master
```
2. 创建/srv/pillar目录
```
sudo mkdir /srv/pillar
```

3. 在pillar的base环境下随便编写1个apahce.sls
+ 语法为Django的template下的python语法
+ 冒号后面一定要有空格!!
```
sudo vim /srv/pillar/apache.sls
###############
{% if grains['os'] == 'CentOS' %}
apache: httpd
{% elif grains['os'] == 'Debian' %}
apache: apache2
{% endif %}
###############
```

4. 执行状态文件(top.sls)
指定哪个minion能使用 
```
sudo vim /srv/pillar/top.sls
#############
base:
  '*':
    - apache
#############
```

5. 检测master端是否能获得minion的pillar数据
```
sudo salt '*' pillar.items
```

6. 通知minion我们已经设置了pillar
```
sudo salt '*' saltutil.refresh_pillar
```

7. 检测
```
sudo salt -I 'apache:httpd' test.ping
```

### 1-0. 问题
#### 1-0-1. 使用`salt '*' test.ping`成功,但使用`salt -I 'apache:httpd' test.ping`却失败
```
linux-node3.example.com:
    Minion did not return. [No response]
linux-node1.example.com:
    Minion did not return. [No response]
```
##### 1-0-1-1. 解决
虽然master端重启了,但是minion端还不知道pillar已经更新过了,所以要通知pillar
```
sudo salt '*' saltutil.refresh_pillar
```

## 2. master端开启pillar
为了避免和筛选minion时冲突,所以如果筛选minion的话,要关闭pillar
### 2-1. master配置文件中开启pillar,并重启master
```
sudo vim /etc/salt/master 
###########
pillar_opts: True
###########
sudo systemctl restart salt-master
```
### 2-2. 检测是否能拿到pillar
```
sudo salt '*' pillar.items
```
