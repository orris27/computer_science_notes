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

rpm -ql logstash

#++++++++++++++++++++++++++++++++++
# 前台执行进行验证
#++++++++++++++++++++++++++++++++++
```
### 1-1. 安装在Elasticsearch的哪个节点
随便哪个节点,老师安装在最一开始的节点,即`192.168.56.10`上面(另一个节点是`192.168.56.20`)

## 2. 使用
+ 插件=>改变输入和输出
+ 事件
+ codec可以理解为编解码器
+ 启动费时
### 2-1. 前台执行
1. 前台交互(Logstash-6.3版本中写不写codec都等于以前写codec了)
```
/usr/share/logstash/bin/logstash -e 'input { stdin{} } output { stdout{} }'
#------------------------------------------------------------------------------------
hello
{
    "@timestamp" => 2018-08-07T10:28:56.862Z,
          "host" => "es-master",
      "@version" => "1",
       "message" => "hello"
}
#------------------------------------------------------------------------------------
```
2. 显示的更加详细
```
/usr/share/logstash/bin/logstash -e 'input { stdin{} } output { stdout{codec => rubydebug} }'
```
3. 将输出发送到Elasticsearch里面 => 刷新原来的Head网页,发生变化,并且点击Browser,刷新,会有源源不断的数据过来
> 语法参考:[Logstash官网output插件文档](https://www.elastic.co/guide/en/logstash/current/plugins-outputs-elasticsearch.html)
```
/usr/share/logstash/bin/logstash -e 'input { stdin{} } output { elasticsearch{hosts => ["192.168.56.10:9200"]} }'
```
4. 将输出发送到Elasticsearch和当前终端里
```
/usr/share/logstash/bin/logstash -e 'input { stdin{} } output { elasticsearch{hosts => ["192.168.56.10:9200"] } stdout {codec => rubydebug}}'
```

5. 利用配置文件将输出发送到Elasticsearch和当前终端里
```

```
## 3. logstash
### 3-1. options
1. `-e`:用字符串充当配置文件
2. `-f`:指定配置文件


### 3-2. 使用插件
#### 3-2-1. ouput插件
output插件可以将输出存放到stdout,redis,elasticsearch等里面
> https://www.elastic.co/guide/en/logstash/current/output-plugins.html


## 4. 配置文件
1. 插件
    1. 输入
    ```
    input {
        #...
    }
    ```
    2. 过滤
    ```
    filter {
        #...
    }
    ```
    3. 输出
    ```
    output {
        #..
    }
    ```

2. 格式
    1. 语法
    + 可以写多个
    + 支持数组
    + 注释:`#`
    ```
    输入/过滤/输出 {
        插件 {
           参数名 => 值
        }
    }
    ```
    2. 模板
    ```
    input {
        插件 {
           参数名 => 值
        }
    }
    output {
        插件 {
           参数名 => 值
        }
    }
    ```
### 4-1. 输入
#### 4-1-1. file
##### 4-1-1-1. 参数
1. `sincedb_path`:记录读到哪里,默认文件根下,为隐藏文件=>如果Logstash挂了可以知道下次从哪里开始读  
2. `start_position`:从文件的哪个地方开始收集.默认是尾部
3. `discover_interval`:多久时间看下被监视的文件.
##### 4-1-1-2. 实例:收集系统日志到Elasticsearch
```
vim /etc/logstash/conf.d/get_messages.conf
##################################################


##################################################
```
### 4-2. 输出
#### 4-2-1. elasticsearch
##### 4-1-1-1. 参数
1. `index`:存放在Elasticserch里的索引的名称
