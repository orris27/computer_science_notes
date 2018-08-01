## 1. 安装
1. 安装依赖包
```
# sudo yum install python-pip -y
sudo pip install python-etcd
sudo pip install etcd
```
2. 常规操作
+ etcd运行在后台
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
+ ttl的单位是秒,下面的代码就是5s后过期
```
curl -s http://172.19.28.82:2379/v2/keys/my_key2 -XPUT -d value="my_value2" -d ttl=5 | python -m json.tool
curl -s http://172.19.28.82:2379/v2/keys/my_key2 | python -m json.tool
```


### 1-0. 问题
1. `-bash: /usr/local/bin/etcd: cannot execute binary file`
+ 我下载了个arm版本,其实应该是amd版本才对

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

### 2-0. 问题
1. 我在etcd设置好后执行`salt '*' pillar.items`却没有获取到设置的<key,valule>
+ 在master的日志文件中查找,如`tail /var/log/salt/master`
2. 不能导入etcd
```
2018-08-01 19:50:39,942 [salt.pillar      ][ERROR   ][11797] Failed to load ext_pillar etcd: (unable to import etcd, module most likely not installed)
Traceback (most recent call last):
  File "/usr/lib/python2.7/site-packages/salt/pillar/__init__.py", line 538, in ext_pillar
    key)
  File "/usr/lib/python2.7/site-packages/salt/pillar/__init__.py", line 508, in _external_pillar_data
    val)
  File "/usr/lib/python2.7/site-packages/salt/pillar/etcd_pillar.py", line 97, in ext_pillar
    client = salt.utils.etcd_util.get_conn(__opts__, profile)
  File "/usr/lib/python2.7/site-packages/salt/utils/etcd_util.py", line 81, in get_conn
    '(unable to import etcd, '
CommandExecutionError: (unable to import etcd, module most likely not installed)
```
+ `sudo pip install etcd`

3. module里面没有Client属性
```
2018-08-01 20:02:26,145 [salt.pillar      ][ERROR   ][14730] Failed to load ext_pillar etcd: 'module' object has no attribute 'Client'
Traceback (most recent call last):
  File "/usr/lib/python2.7/site-packages/salt/pillar/__init__.py", line 538, in ext_pillar
    key)
  File "/usr/lib/python2.7/site-packages/salt/pillar/__init__.py", line 508, in _external_pillar_data
    val)
  File "/usr/lib/python2.7/site-packages/salt/pillar/etcd_pillar.py", line 97, in ext_pillar
    client = salt.utils.etcd_util.get_conn(__opts__, profile)
  File "/usr/lib/python2.7/site-packages/salt/utils/etcd_util.py", line 78, in get_conn
    return etcd.Client(host, port)
AttributeError: 'module' object has no attribute 'Client'
```
