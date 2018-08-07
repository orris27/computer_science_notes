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
        1. + 可以写多个
        2. + 支持数组
        3. + 注释:`#`
        4. 值
            + 数字
            + 字符串(双引号)
            + 数组
        5. 对输入做if判断
            1. 基于字段,尤其是type
            2. 不同的插件收集到的数据放到不同的输出源里面
    ```
    输入/过滤/输出 {
        #if [type] == "xxx" { # 一般放在输出里.中括号里面没有双引号
            插件 {
                参数名 => 值
            }
        #}
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
> https://www.elastic.co/guide/en/logstash/current/plugins-inputs-file.html
##### 4-1-1-1. 参数
1. `sincedb_path`:记录读到哪里,默认文件根下,为隐藏文件=>如果Logstash挂了可以知道下次从哪里开始读  
2. `start_position`:从文件的哪个地方开始收集.默认是尾部,如`"end"`
3. `discover_interval`:多久时间看下被监视的文件,默认15秒
4. `path`:指定哪个文件.支持通配符`*`,如`"/var/log/*.log"`
5. `type`:主要用在filter行为上. ≠Elasticsearch里面的`_type`. 如`"system"`
6. `codec`
    



#### 4-1-3. codec
> https://www.elastic.co/guide/en/logstash/current/codec-plugins.html
1. 编解码器
2. 可以处理字符串
3. 可以放在input下的任何模块里,比如file,stdin等.
##### 4-1-3-1. multiline
+ 可以用来处理多行数据
+ 匹配字符串成功后,不会马上输出,而是要等下一行(因为要看下一行是否要合并进来)
###### 4-1-3-1-1. options
1. `pattern`:正则表达式,匹配字符串.在什么情况下合并起来
2. `negate`:是匹配成功的执行what,还是匹配不成功的执行what.`true`或`false`
3. `what`:对匹配成功(`negate=false`)/匹配失败(`negate=true`)的事件处理.那么是合并到上面还是下面.`"previous"`或`"next"`
###### 4-1-3-1-2. 实例
1. 模板
```
input {
  stdin {
    codec => multiline {
      pattern => "pattern, a regexp"
      negate => true / false
      what => "previous" / "next"
    }
  }
}
```
2. 对于下面的语段进行合并
```
[orris]hello
my name is orris
are you ok?
[mirai]hi
I am mirai
[orris]bye
[mirai]bye
```
=>
```
# 如果开头不是左中括号的话,就合并到上面
input {
  stdin {
    codec => multiline {
      pattern => "^\["
      negate => true
      what => "previous"
    }
  }
}

```

### 4-3. 输出
#### 4-3-1. elasticsearch
> https://www.elastic.co/guide/en/logstash/current/plugins-outputs-elasticsearch.html
##### 4-3-1-1. 参数
1. `hosts`:指定Elasticsearch.如`["192.168.56.10:9200"]`
2. `index`:存放在Elasticserch里的索引的名称.可以支持日期格式,如`"logstash-%{+YYYY.MM.dd}"`.等于Elasticsearch里面的`_index`


### 4-4. 实例
1. 收集系统日志到Elasticsearch和stdout
```
vim /etc/logstash/conf.d/get_messages.conf
##################################################
input {
    file {
        path => "/var/log/messages"
        type => "system"
        start_position => "end"
    }
}
output {
    elasticsearch {
        hosts =>  ["192.168.56.10:9200"]
        index => "system-%{+YYYY.MM.dd}"
    }
    stdout {
        codec => rubydebug
    }
}
##################################################
/usr/share/logstash/bin/logstash -f /etc/logstash/conf.d/get_messages.conf

#+++++++++++++++++++++++++++++++++++++++++++++++++
# 之后通过Head检查下就行了,注意发现间隔是15秒,所以要等一会
#+++++++++++++++++++++++++++++++++++++++++++++++++
```
2. 收集系统日志和Java日志(Elasticsearch的)到Elasticsearch的不同index里面
```
vim /etc/logstash/conf.d/if.conf
##################################################
input {
    file {
        path => "/var/log/nginx/nginx-acces.log"
        type => "nginx-access"
        start_position => "beginning"
        codec => "json"
    }
}
output {
    stdout {
        codec => rubydebug
    }
}
##################################################
/usr/share/logstash/bin/logstash -f /etc/logstash/conf.d/if.conf

#+++++++++++++++++++++++++++++++++++++++++++++++++
# 之后通过Head检查下就行了,注意发现间隔是15秒,所以要等一会
#+++++++++++++++++++++++++++++++++++++++++++++++++
```

3. 收集系统日志和Java日志(Elasticsearch的)到Elasticsearch的不同index里面,并且Java日志多行处理成一行(一个事件)
```
vim /etc/logstash/conf.d/multiline.conf
##################################################
input {
    file {
        path => "/var/log/messages"
        type => "system"
        start_position => "end"
    }
    file {
        path => "/var/log/elasticsearch/oldboy.log"
        type => "java"
        start_position => "end"
        codec => multiline {
            pattern => "^\["
            negate => true
            what => "previous"
        }
    }
}
output {
    if [type] == "system" {
        elasticsearch {
            hosts =>  ["192.168.56.10:9200"]
            index => "system-%{+YYYY.MM.dd}"
        }
    }
    if [type] == "java" {
        elasticsearch {
            hosts =>  ["192.168.56.10:9200"]
            index => "java-%{+YYYY.MM.dd}"
        }
    }
    stdout {
        codec => rubydebug
    }
}

##################################################
/usr/share/logstash/bin/logstash -f /etc/logstash/conf.d/multiline.conf

#+++++++++++++++++++++++++++++++++++++++++++++++++
# 之后通过Head检查下就行了,注意如果收集到的日志太多的话,Web页面是看不到的.可以利用Browser标签下面左侧的搜索工具查找.注意发现间隔是15秒,所以要等一会
#+++++++++++++++++++++++++++++++++++++++++++++++++
```
4. 在上面的基础上,再获取用JSON格式记录访问日志的Nginx的日志
    + 标准化的日志是统一目录,统一命名规范
    + 不推荐正则表达式:麻烦+调试难+效率低
    1. 修改Nginx的配置文件,使Nginx用JSON格式记录访问日志
    2. 写监控Nginx得到访问日志的Logstash
        1. 输入是访问日志
        2. 用json格式解码
        3. 输出到Elasticsearch
    3. 启动Logstash
    4. 使用Head或Kibana查看收集到的数据
```
vim /etc/nginx/nginx.conf
#####################################################
http {
    # ...
    log_format json_log_name '{ "@timestamp": "$time_local", '
        '"remote_addr": "$remote_addr", '
        '"referer": "$http_referer", '
        '"request": "$request", '
        '"status": $status, '
        '"bytes": $body_bytes_sent, '
        '"agent": "$http_user_agent", '
        '"x_forwarded": "$http_x_forwarded_for", '
        '"up_addr": "$upstream_addr",'
        '"up_host": "$upstream_http_host",'
        '"up_resp_time": "$upstream_response_time",'
        '"request_time": "$request_time"'
        ' }';
    server {
        # ...
        access_log /var/log/nginx/access-json.log json_log_name;
    }
}
#####################################################
nginx -s reload

vim /etc/logstash/conf.d/json.conf
##################################################
input {
    file {
        path => "/var/log/messages"
        type => "system"
        start_position => "end"
    }
    file {
        path => "/var/log/elasticsearch/oldboy.log"
        type => "java"
        start_position => "end"
        codec => multiline {
            pattern => "^\["
            negate => true
            what => "previous"
        }
    }
    file {
	path => "/var/log/nginx/access-json.log"
	type => "nginx-access"
	start_position => "beginning"
	codec => "json"
    }
}
output {
    if [type] == "system" {
        elasticsearch {
            hosts =>  ["192.168.56.10:9200"]
            index => "system-%{+YYYY.MM.dd}"
        }
    }
    if [type] == "java" {
        elasticsearch {
            hosts =>  ["192.168.56.10:9200"]
            index => "java-%{+YYYY.MM.dd}"
        }
    }
    if [type] == "nginx-access" {
        elasticsearch {
            hosts =>  ["192.168.56.10:9200"]
            index => "nginx-access-%{+YYYY.MM.dd}"
        }
    }
    stdout {
        codec => rubydebug
    }
}


##################################################
/usr/share/logstash/bin/logstash -f /etc/logstash/conf.d/json.conf

#+++++++++++++++++++++++++++++++++++++++++++++++++
# 之后通过Head/Kibana检查下就行了,注意Nginx写日志有缓存,不会马上写入,而且发现间隔也是15秒,所以可能要等待
#+++++++++++++++++++++++++++++++++++++++++++++++++
```

4. 将系统日志写到指定端口,然后我们的Logstash直接从该端口拿日志内容
	> 原理: 启动Logstash的syslog插件,让syslog插件去监听514端口.然后rsyslog服务将日志推送到514端口上
```
vim /etc/logstash/conf.d/syslog.conf
# syslog插件默认监听514端口
################################
input {
	syslog {
		type => "system-syslog"
		host => "192.168.56.10"
		port => "514"
	}
}
output {
	if [type] == "system-syslog" {
		elasticsearch {
            hosts =>  ["192.168.56.10:9200"]
            index => "system-syslog-%{+YYYY.MM.dd}"
		} 
	}
	stdout {
		codec => "rubydebug"
	}
}
################################

vim /etc/rsyslog.conf
################################
*.* @@192.168.56.10:514 # 在最下面被注释的一块
################################
systemctl restart rsyslog
/usr/share/logstash/bin/logstash -f /etc/logstash/conf.d/syslog.conf
#+++++++++++++++++++++++++++++++++++++++++++++++++
# 之后通过Head/Kibana检查下就行了
#+++++++++++++++++++++++++++++++++++++++++++++++++
```
