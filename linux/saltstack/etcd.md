## 1. 安装
```
cd ~/tools/
wget https://github.com/coreos/etcd/releases/download/v3.3.9/etcd-v3.3.9-linux-arm64.tar.gz
tar zxf etcd-v3.3.9-linux-arm64.tar.gz
cd etcd-v3.3.9-linux-arm64
sudo cp etcd etcdctl /usr/local/bin/
etcd --version

mkdir -p /data/etcd

```


```
etcd --name auto_scale --date-dir /data/etcd/ \
--listen-peer-urls 'http://172.19.28.82:2380,http://172.19.28.82:7001' \
--listen-client-urls 'http://172.19.28.82:2379,http://172.19.28.82:4001' \
--advertise-client-urls 'http://172.19.28.82:2379,http://172.19.28.82:4001' \
```
