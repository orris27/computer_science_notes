## 1. 安装
Elasticsearch的最好安装方法就是yum安装. > [官方rpm安装文档](https://www.elastic.co/guide/en/elasticsearch/reference/current/rpm.html)
+ 参数的调整其实最终还是对JVM的调优
1. 安装Java
+ ES是Java开发的

2. 导入GPG的key
3. 配置yum仓库
4. yum安装

```
yum install -y java
java -version # 验证
```
