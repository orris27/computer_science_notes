## 1. 安装
> [官方rpm安装文档](https://www.elastic.co/guide/en/logstash/current/installing-logstash.html)
1. 安装Java
2. 导入GPG的key
3. 配置yum仓库
4. yum安装
5. 验证
```
yum install -y java
java -version # 验证

cat > /etc/yum.repos.d/logstash.repo <<EOF
[logstash-6.x]
name=Elastic repository for 6.x packages
baseurl=https://artifacts.elastic.co/packages/6.x/yum
gpgcheck=1
gpgkey=https://artifacts.elastic.co/GPG-KEY-elasticsearch
enabled=1
autorefresh=1
type=rpm-md
EOF

sudo yum install logstash -y


#++++++++++++++++++++++++++++++++++
# 前台执行进行验证
#++++++++++++++++++++++++++++++++++
```
### 1-1. 安装在Elasticsearch的哪个节点
随便哪个节点,老师安装在最一开始的节点,即`192.168.56.10`上面(另一个节点是`192.168.56.20`)

## 2. 使用
+ 插件=>改变输入和输出
+ ruby可以对事件来处理,codec可以理解为编解码器

### 2-1. 前台执行
```
/opt/logstash/bin/logstash -e 'input { stdin{} } output { stdout{} }'

# 显示的更加详细
/opt/logstash/bin/logstash -e 'input { stdin{} } output { stdout{codec => rubydebug} }'

# 将输出发送到Elasticsearch里面 => 刷新原来的Head网页,发生变化,并且点击数据浏览,刷新,会有源源不断的数据过来
/opt/logstash/bin/logstash -e 'input { stdin{} } output { elasticsearch{hosts => ["192.168.56.10:9200"] }'

# 将输出发送到Elasticsearch和当前终端里
/opt/logstash/bin/logstash -e 'input { stdin{} } output { elasticsearch{hosts => ["192.168.56.10:9200"] } stdout {codec => rubydebug}'

```
