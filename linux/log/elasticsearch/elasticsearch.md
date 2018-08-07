## 1. 安装
Elasticsearch的最好安装方法就是yum安装. > [官方rpm安装文档](https://www.elastic.co/guide/en/elasticsearch/reference/current/rpm.html)
+ 参数的调整其实最终还是对JVM的调优
1. 安装Java
+ ES是Java开发的
2. 导入GPG的key
3. 配置yum仓库
4. yum安装
5. 修改ES配置文件和`limits.conf`参数
6. 启动服务
7. 验证
```
yum install -y java
java -version # 验证

rpm --import https://artifacts.elastic.co/GPG-KEY-elasticsearch


# Create a file called elasticsearch.repo in the /etc/yum.repos.d/ directory for RedHat based distributions, or in the /etc/zypp/repos.d/ directory for OpenSuSE based distributions, containing:
#############################################################
[elasticsearch-6.x]
name=Elasticsearch repository for 6.x packages
baseurl=https://artifacts.elastic.co/packages/6.x/yum
gpgcheck=1
gpgkey=https://artifacts.elastic.co/GPG-KEY-elasticsearch
enabled=1
autorefresh=1
type=rpm-md
#############################################################
sudo yum install elasticsearch

#mkdir -p /data/es-data
#chown -R elasticsearch:elasticsearch /data/es-data

cd /etc/elasticsearch
vim elasticsearch.yml
#############################################################
cluster.name: oldboy # 名称不一样就不是一个集群,日志也是根据
node.name: linux-node1 

path.data: /var/lib/elasticsearch
path.logs: /var/log/elasticsearch

bootstrap.mlockall: true 

network.host: 0.0.0.0 
http.port: 9200
#############################################################
#systemctl start elasticsearch
#systemctl status elasticsearch # 只能用elasticsearch账户启动


vim /etc/security/limits.conf 
################################################################
#*               soft    core            0
#*               hard    rss             10000
#@student        hard    nproc           20
#@faculty        soft    nproc           20
#@faculty        hard    nproc           50
#ftp             hard    nproc           0
#@student        -       maxlogins       4
*                hard    memlock         unlimited # 这个不要自己输入,而是取消注释!!(也可能是和顺序有关)
*                soft    memlock         unlimited
*                soft nofile 65536
*                hard nofile 65536
*                soft nproc 2048
*                hard nproc 4096
################################################################

su -s /bin/sh -c "/usr/share/elasticsearch/bin/elasticsearch -d" elasticsearch # -d在后台运行
#su -s /bin/sh -c "/usr/share/elasticsearch/bin/elasticsearch" elasticsearch 
# 停止/关闭服务的话,直接ps aux | grep elastic,然后kill <最长的进程名的pid>就行了=>2个elastic进程都会被杀死

curl 192.168.56.10:9200 # 出现结果就说明成功了,不过需要等一下

# 浏览器可以访问"192.168.56.10:9200"验证

```

### 1-0. 报错
> https://github.com/orris27/orris/blob/master/linux/installation_error.md

## 2. 交互
### 2-1. Java API
### 2-2. RESTful API
+ 查询的东西
+ `pretty`让数据更美观
```
curl -i -XGET 'http://192.168.56.10:9200/_count?pretty' -d '
"query": {
  "match_all": {}
}
}'
```
## 3. 插件
官方有个Web插件来管理
```
cd /usr/share/elasticsearch
sudo bin/plugin install license
sudo bin/plugin install marvelous-agent

```
