## 1. ab
### 1-1. 输出
```
This is ApacheBench, Version 2.3 <$Revision: 1826891 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 10.0.0.7 (be patient)
Completed 100 requests
Completed 200 requests
Completed 300 requests
Completed 400 requests
Completed 500 requests
Completed 600 requests
Completed 700 requests
Completed 800 requests
Completed 900 requests
Completed 1000 requests
Finished 1000 requests


Server Software:        nginx/1.14.0 # 请求的服务器版本
Server Hostname:        10.0.0.7
Server Port:            80 # 请求的端口

Document Path:          /
Document Length:        612 bytes # 响应体的长度.一般为100k左右

Concurrency Level:      100   # 当前并发用户数
Time taken for tests:   0.123 seconds    # 测试的总共时间(所有请求话费的时间)
Complete requests:      1000
Failed requests:        0 # 失败的请求数
Total transferred:      845000 bytes # 所有请求得到的响应数据总量
HTML transferred:       612000 bytes # 所有请求得到的响应体数据总量(不包括响应头)=>612000=612*1000
Requests per second:    8138.75 [#/sec] (mean) # 吞吐率.每秒钟处理的请求数.8138.75=1000/0.123
Time per request:       12.287 [ms] (mean) # 响应时间.平均用户请求等待时间(一个请求发出到收到响应的时间,然后对1000个请求取平均值的结果)
Time per request:       0.123 [ms] (mean, across all concurrent requests) # 平均请求处理时间(这里是指处理,上面是指用户等待)
Transfer rate:          6716.06 [Kbytes/sec] received # 每秒获得数据长度.=>计算带宽就是6*8M(带宽的单位是bit,所以要乘8)

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    3   2.6      2      13
Processing:     2    9   2.9      8      24
Waiting:        1    7   2.6      6      23
Total:          5   11   5.1     10      31

Percentage of the requests served within a certain time (ms)
  50%     10
  66%     10
  75%     10
  80%     10
  90%     23
  95%     27
  98%     27
  99%     28
 100%     31 (longest request)

```
