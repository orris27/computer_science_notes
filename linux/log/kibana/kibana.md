## 1. 安装
1. 压缩包安装
    1. 下载压缩包
    2. 解压压缩包
    3. 移动压缩包到`/usr/local`下
    4. 建立软连接
2. 修改配置文件
    + 见下面代码
3. 启动Kibana
> [官方文档](https://www.elastic.co/guide/en/kibana/current/install.html)
```
wget https://artifacts.elastic.co/downloads/kibana/kibana-6.3.2-linux-x86_64.tar.gz
yum install -y perl-Digest-SHA
shasum -a 512 kibana-6.3.2-linux-x86_64.tar.gz
tar -xzf kibana-6.3.2-linux-x86_64.tar.gz
mv kibana-6.3.2-linux-x86_64 /usr/local/
ln -s /usr/local/kibana-6.3.2-linux-x86_64/ /usr/local/kibana

vim /usr/local/kibana/config/kibana.yml
#################################################
server.port :5601
server.host: "0.0.0.0"
elasticsearch.url: "http://192.168.56.10:9200"

kibana.index: ".kibana" # kibana需要有个存放数据的地方,所以干脆直接存在ES里面了
#################################################


yum install screen -y 
screen 
/usr/local/kibana/bin/kibana
Ctrl+a+d
screen -ls

#+++++++++++++++++++++++++++++++++
# 浏览器访问192.168.56.10:5601
#+++++++++++++++++++++++++++++++++


```
## 2. 使用
1. 点击Management>Index Patterns>Create Index Pattern,创建一个index去跟踪
    1. 填写index名字
        + 如`es-error-*`.用`*`会匹配时间
    2. 选择`@timestamp`
2. 到Discover里面,右上角有时间选择,默认是过去的15分钟,修改成比如说今天,这样就会有数据显示出来了
    > 我点了Today没有显示出数据,是因为点的时候是`00:10`,确实今天没有数据...





