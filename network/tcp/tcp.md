## 1. 数据包
### 1-1. 结构
1. [TCP数据包结构中文](https://img-blog.csdn.net/20130629171810234?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvZnp6bW91c2U=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center) + [TCP数据包结构英文](https://img-blog.csdn.net/20130629171829718?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvZnp6bW91c2U=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center)

2. 说明
    1. 16位源端口号
    2. 16位目的端口号
    3. 32位序列号
    4. 32位确认号
    5. 4位首部长度
    6. 6位保留
    7. 6位标志位
        1. URG：紧急指针有效
        2. ACK：确认序号有效
        3. PSH：接收方尽快将这个报文段交给应用层
        4. RST：重建连接
        5. SYN：同步序号用来发起一个连接.标志位置1
        6. FIN：发端完成发送任务
    8. 16位窗口大小
    9. 16位TCP检验和
    10. 16位紧急指针
    11. 选项(若有)
    12. 数据(若有)


## 2. socket
### 2-2. 状态
1. 查看当前系统的TCP-Socket情况
> 可以看到TCP-socket由四个信息组成,源IP,源PORT,目的IP和目的PORT.下图中10.0.0.7服务器(本机)启动了redis服务,因此存在源IP和PORT是10.0.0.7:6379的socket
```
[root@lbs07 ~]# netstat -nat
Active Internet connections (servers and established)
Proto Recv-Q Send-Q Local Address           Foreign Address         State      
tcp        0      0 10.0.0.7:6379           0.0.0.0:*               LISTEN     
tcp        0      0 0.0.0.0:22              0.0.0.0:*               LISTEN     
tcp        0      0 0.0.0.0:4505            0.0.0.0:*               LISTEN     
tcp        0      0 127.0.0.1:25            0.0.0.0:*               LISTEN     
tcp        0      0 0.0.0.0:4506            0.0.0.0:*               LISTEN     
tcp        0      0 10.0.0.7:4505           10.0.0.7:50208          ESTABLISHED
tcp        0      0 10.0.0.7:22             10.0.0.1:60114          ESTABLISHED
tcp        0      0 10.0.0.7:4506           10.0.0.7:40892          ESTABLISHED
tcp        0      0 10.0.0.7:50208          10.0.0.7:4505           ESTABLISHED
tcp        0      0 10.0.0.7:40892          10.0.0.7:4506           ESTABLISHED
tcp6       0      0 :::80                   :::*                    LISTEN     
tcp6       0      0 :::22                   :::*                    LISTEN     
tcp6       0      0 ::1:25                  :::*                    LISTEN     
```
2. 如果socket建立了连接,那么便会变成ESTABLISHED状态
> 我在10.0.0.8的电脑上连接了10.0.0.7的redis服务.那么在10.0.0.7上查看socket情况时,就会出现`tcp        0      0 10.0.0.7:6379           10.0.0.8:45944          ESTABLISHED`这样的结果.可以看到10.0.0.8也开了个随机端口45944来保持和redis服务的长连接
```
[root@lbs07 ~]# netstat -nat
Active Internet connections (servers and established)
Proto Recv-Q Send-Q Local Address           Foreign Address         State      
tcp        0      0 10.0.0.7:6379           0.0.0.0:*               LISTEN     
tcp        0      0 0.0.0.0:22              0.0.0.0:*               LISTEN     
tcp        0      0 0.0.0.0:4505            0.0.0.0:*               LISTEN     
tcp        0      0 127.0.0.1:25            0.0.0.0:*               LISTEN     
tcp        0      0 0.0.0.0:4506            0.0.0.0:*               LISTEN     
tcp        0      0 10.0.0.7:6379           10.0.0.8:45944          ESTABLISHED
tcp        0      0 10.0.0.7:4505           10.0.0.7:50208          ESTABLISHED
tcp        0      0 10.0.0.7:22             10.0.0.1:60114          ESTABLISHED
tcp        0      0 10.0.0.7:4506           10.0.0.7:40892          ESTABLISHED
tcp        0      0 10.0.0.7:50208          10.0.0.7:4505           ESTABLISHED
tcp        0      0 10.0.0.7:40892          10.0.0.7:4506           ESTABLISHED
tcp6       0      0 :::80                   :::*                    LISTEN     
tcp6       0      0 :::22                   :::*                    LISTEN     
tcp6       0      0 ::1:25                  :::*                    LISTEN     
```

## 3. TCP优化
### 3-1. 端口不够
#### 3-1-1. 思路
1. 快速回收+重用=>解决TIME_WAIT=>解决端口不够
```
echo "0" > /proc/sys/net/ipv4/tcp_tw_recycle # 快速回收不推荐打开,因为可能有安全问题
echo "1" > /proc/sys/net/ipv4/tcp_tw_reuse 
```
2. 主动关闭socket的一端(=TIME_WAIT多的一端)增加IP=>解决端口不够
    1. redis(只是一个例子)
        + 比如说我们在eth0:0上新增10.0.0.10,然后让redis启动在10.0.0.10,那么就会使用这个IP来连接了
    2. Haproxy:
        + `serve .. check source <ip2>:....`:使用source关键字就行了
3. LB如果和后台服务器建立长连接的话,也可以同时减少两边的TIME_WAIT的socket
    > [Nginx和后台服务器保持长连接的方法](https://github.com/orris27/orris/blob/master/linux/nginx/nginx.md)
#### 3-1-2. 概念
1. `tcp_timestamp`
    + 支持TIME_WAIT
2. `tcp_rw_recycle`: 快速回收
    + 如果客户端工作在NAT中的话,可能是会有问题的=>为了安全,服务端不要开快速回收
    + 我们还是不要开了,因为安全有点问题
3. `tcp_tw_reuse`: 重用
    + 默认是关闭的.直接拿TIME_WAIT的数据包来工作,当然可能也会有保留,所以可能比较安全
    + 我们可以开,是安全的
4. 结论
    1. 关闭快速回收
    2. 开启重用
    
### 3-2. 用Nginx和curl来测试socket的内部工作机制
1. 用户用curl去请求Web的时候,用户端开启了随机端口
2. 用户端的随机端口连接的是Web的80端口(多对一)
3. 随机端口的范围的确是由`ip_local_port_range`来控制的
4. Linux里TIME_WAIT=2MSL=1min
#### 3-2-1. 实例
1. `10.0.0.7`去curl `10.0.0.8`的话,查看socket的情况
```
curl 10.0.0.7

[root@lbs07 ~]# netstat -nat | grep 'TIME_WAIT' # 10.0.0.7处有TIME_WAIT的套接字
Proto Recv-Q Send-Q Local Address           Foreign Address         State      
tcp        0      0 10.0.0.7:34846          10.0.0.8:80             TIME_WAIT  
[root@master-8 ~]# netstat -nat | grep 'TIME_WAIT' # 10.0.0.8没任何TIME_WAIT的套接字
```
> 1. 说明curl是在本地随机生成一个34846这样的端口,然后去连接Web服务器的80端口,建立连接
> 2. 简单的Web连接是用户主动关闭的 => TIME_WAIT在用户
> 3. 产生的随机端口是在用户端
> 4. 综合2和3,用户产生随机端口并且处于TIME_WAIT => 用户会有端口不够的问题
> 5. 从4中得出,代理服务器去请求后台服务器就是这里的用户端 => 代理服务器会有端口不够的

2. 缩短随机端口的分配范围,查看系统的行为
```
############################################################################
# 10.0.0.7
############################################################################
echo 'net.ipv4.ip_local_port_range = 60000 60010' >>/etc/sysctl.conf
sysctl -p

for i in `seq 15`;do curl 10.0.0.8;done
#----------------------------------------------------------------------------
10.0.0.8:80
10.0.0.8:80
10.0.0.8:80
10.0.0.8:80
10.0.0.8:80
10.0.0.8:80
10.0.0.8:80
10.0.0.8:80
10.0.0.8:80
10.0.0.8:80
10.0.0.8:80
curl: (7) Failed to connect to 10.0.0.8: Cannot assign requested address
curl: (7) Failed to connect to 10.0.0.8: Cannot assign requested address
curl: (7) Failed to connect to 10.0.0.8: Cannot assign requested address
curl: (7) Failed to connect to 10.0.0.8: Cannot assign requested address
#----------------------------------------------------------------------------
```
> 1. 说明随机端口的范围的确是由`ip_local_port_range`来控制的
> 2. 如果端口不够就无法正确完成命令
