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
# 这个请求不能成功
curl -i -XGET 'http://192.168.56.10:9200/_count?pretty' -d \
'{
"query": {
  "match_all": {}
}
}'
```
## 3. 插件
官方有个Marvel插件来管理
### 3-1. Marvel
#### 3-1-1. 安装
> 官方文档: https://www.elastic.co/downloads/x-pack
1. ES v5以下版本
```
cd /usr/share/elasticsearch
sudo bin/plugin install license
sudo bin/plugin install marvelous-agent
```
2. ES v5
+ 安装X-pack,因为Marvel已经被融合进去了
```
cd /usr/share/elasticsearch
bin/elasticsearch-plugin install x-pack
#----------------------------------------------------------------------------------
ERROR: this distribution of Elasticsearch contains X-Pack by default
#----------------------------------------------------------------------------------
```
3. ES v6
+ The following installation instructions are only valid for versions 6.2 and older. In versions 6.3 and later, X-Pack is included with the default distributions of Elastic Stack, with all free features enabled by default. An opt-in trial is available to enable 
+ 不用自己安装了

### 3-2. head插件(推荐)
#### 3-2-1. 安装
>　官方文档:https://github.com/mobz/elasticsearch-head
+ head插件在ES v5以上就不作为head插件来使用了,而必须是单独的head服务才行
1. 安装Head服务
2. 连接Head和Elasticseach
    1. 使Elasticsearch允许跨域请求,并设置跨域请求的范围
    2. 设置身份验证信息(可选)
3. 使用Head服务自己监听的9100端口来访问
```
git clone git://github.com/mobz/elasticsearch-head.git
cd elasticsearch-head
npm install
npm run start


vim /etc/elasticsearch/elasticsearch.yml
##################################################
http.cors.enabled: true
http.cors.allow-origin: "*"
##################################################

#Basic Authentication
#elasticsearch-head will add basic auth headers to each request if you pass in the correct url parameters
#You will also need to add http.cors.allow-headers: Authorization to the elasticsearch configuration


# 浏览器访问http://192.168.56.10:9100/

```

#### 3-2-2. 使用
##### 3-2-2-1. 基本操作
1. 浏览器访问`192.168.56.10:9200/_plugin/head/`
2. 点击复合查询
3. 在"查询"里填写信息
##### 3-2-2-2. 设置数据
1. URL:`http://192.168.56.10:9200/index-demo/test`
+ Query第一行不能是`localhost`
+ Query第二行不能以`/`开头
2. 请求方式:`POST`
3. 数据:
```
{
  "user": "oldboy",
  "mesg": "hello world"
}
```
4. 发送请求
5. 获得`"_id"`,如`W5ZmE2UB4D62RSdOrpQM`
```
{
  "_index": "index-demo",
  "_type": "test",
  "_id": "W5ZmE2UB4D62RSdOrpQM",
  "_version": 1,
  "result": "created",
  "_shards": {
    "total": 2,
    "successful": 1,
    "failed": 0
    },
  "_seq_no": 1,
  "_primary_term": 1
}
```
##### 3-2-2-3. 请求某个id的数据
1. URL:`http://192.168.56.10:9200/index-demo/test/<_id>`
+ 如`http://192.168.56.10:9200/index-demo/test/W5ZmE2UB4D62RSdOrpQM`
2. 请求方式:`GET`
3. 数据:
```
{
  "user": "oldboy",
  "mesg": "hello world"
}
```
4. 发送请求
5. 从`_source`处可以获取到刚才提交的数据
```
#-------------------------------------------------------
{
  "_index": "index-demo",
  "_type": "test",
  "_id": "W5ZmE2UB4D62RSdOrpQM",
  "_version": 1,
  "found": true,
  "_source": {
    "user": "oldboy",
    "mesg": "hello world"
    }
}
#-------------------------------------------------------
```
##### 3-2-2-4. 删除某个id的数据
1. URL:`http://192.168.56.10:9200/index-demo/test/<_id>`
+ 如`http://192.168.56.10:9200/index-demo/test/W5ZmE2UB4D62RSdOrpQM`
2. 请求方式:`DELETE`
3. 数据:
```
{
  "user": "oldboy",
  "mesg": "hello world"
}
```
4. 发送请求
5. 从`_source`处可以获取到刚才提交的数据
```
#-------------------------------------------------------
{
  "_index": "index-demo",
  "_type": "test",
  "_id": "W5ZmE2UB4D62RSdOrpQM",
  "_version": 2,
  "result": "deleted",
  "_shards": {
    "total": 2,
    "successful": 1,
    "failed": 0
    },
  "_seq_no": 2,
  "_primary_term": 1
}
#-------------------------------------------------------
```
