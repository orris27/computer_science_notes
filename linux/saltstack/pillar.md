## 1. 设置pillar
1. master配置文件,并重启
+ top.sls一般就放在base环境下
+ 关闭pillar_opts: False
- 防止获得master的pillar.items
```
sudo vim /etc/salt/master
##############
pillar_root:
  base
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
```
sudo vim /srv/pillar/apache.sls
###############
{% if grains['os'] == 'CentOS' %}
apache:httpd
{% elif grains['os'] == 'Debian' %}
apache:apache2
{% endif %}
###############
```

4. top.sls
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
