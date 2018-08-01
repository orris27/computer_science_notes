## 1. 安装

1. 安装依赖包

```
# sudo yum install python-pip -y
sudo pip uninstall urllib3
sudo pip install urllib3 --upgrade
sudo pip install python-etcd
python
###################
import etcd
etcd.Client() # 只要没有提示说'module' object has no attribute 'Client',就是正确了
###################
```

2. 常规操作
- etcd运行在后台

```
cd ~/tools/
wget https://github.com/coreos/etcd/releases/download/v3.3.9/etcd-v3.3.9-linux-amd64.tar.gz
tar zxf etcd-v3.3.9-linux-amd64.tar.gz
cd etcd-v3.3.9-linux-amd64
sudo cp etcd etcdctl /usr/local/bin/
etcd --version

sudo mkdir -p /data/etcd
sudo nohup etcd --name auto_scale --data-dir /data/etcd/ \
--listen-peer-urls 'http://172.19.28.82:2380,http://172.19.28.82:7001' \
--listen-client-urls 'http://172.19.28.82:2379,http://172.19.28.82:4001' \
--advertise-client-urls 'http://172.19.28.82:2379,http://172.19.28.82:4001' &

sudo netstat -lntup | grep etcd
```

## 2.操作

1. set key

```
curl -s http://172.19.28.82:2379/v2/keys/my_key1 -XPUT -d value="my_value1" | python -m json.tool
```

2. get key

```
curl -s http://172.19.28.82:2379/v2/keys/my_key1 | python -m json.tool
```

3. delete key

```
curl -s http://172.19.28.82:2379/v2/keys/my_key1 -XDELETE  | python -m json.tool
```

4. 设置过期时间

- ttl的单位是秒,下面的代码就是5s后过期

```
curl -s http://172.19.28.82:2379/v2/keys/my_key2 -XPUT -d value="my_value2" -d ttl=5 | python -m json.tool
curl -s http://172.19.28.82:2379/v2/keys/my_key2 | python -m json.tool
```

### 1-0. 问题

1. `-bash: /usr/local/bin/etcd: cannot execute binary file`

- 我下载了个arm版本,其实应该是amd版本才对

## 2. SaltStack配置etcd

1. 告诉master我们的pillar是从etcd那里获取的

```
sudo vim /etc/salt/master
###########################
etcd_pillar_config:
  etcd.host: 172.19.28.82
  etcd.port: 4001

ext_pillar:
  - etcd: etcd_pillar_config root=/salt/haproxy/
###########################
sudo systemctl restart salt-master
```

2. 检测etcd是否正常工作

```
curl -s http://172.19.28.82:2379/v2/keys/salt/haproxy/backend_www_oldboyedu_com/web-node1 -XPUT -d value="172.19.28.82:8080" | python -m json.tool
salt '*' pillar.items
```

3. 检测haproxy是否正常工作
    1. 在浏览器中输入`http://47.100.185.187:8888/haproxy-status`,看能否正常访问
4. 配置haproxy的配置文件
    1. 将配置文件用jinja变量获取节点
    2. 告诉状态文件我们的实体文件要用jinja变量
    + 只要添加`- template: jinja`就可以了
```
vim /srv/salt/prod/cluster/haproxy-outside.sls # 我们发现watch了haproxy的配置文件,因此不用我们手动重启
vim /srv/salt/prod/cluster/files/haproxy-outside.cfg 
###########################
{% for web,web_ip in pillar.backend_www_oldboyedu_com.iteritems() %}
server {{ web }} {{ web_ip }} check inter 2000 rise 30 fall 15
{% endfor %}
###########################

vim /srv/salt/prod/cluster/haproxy-outside.sls

#######################
haproxy-service:
  file.managed:
    - name: /etc/haproxy/haproxy.cfg
    - template: jinja 
#######################
```
5. 执行状态文件
+ 必须保证所有minion都能获取到web-node的值
```
salt '*' state.highstate
```



### 2-0. 问题

> https://github.com/orris27/orris/blob/master/linux/installation_error.md
