## 1. 安装
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
