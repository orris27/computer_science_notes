## 1. 优化
如何分配JVM的内存空间


### 1-1. Heap
> [JVM堆结构图](http://debugo.com/wp-content/uploads/2014/12/jvm_mem.jpg)
-Xmx 4000M 最大堆内存的大小
默认物理内存的1/4,实际中建议不大于4GB

-Xms 4000M 最小堆内存的大小 一开始就分配指定内存大熊啊
默认是物理内存的1/64(<1GB)
堆内存动态伸缩会消耗资源=>建议设置成相同,即-Xms=-Xmx<=4GB

整个堆大小=Young大小+Old大小+Permanent大小

Young区=1个eden区+2个Survivor区(e+s0+s1)

-Xmn Young区大小 (1.4之前是-XX)
比如配置成100M,那么就被"1个eden区+2个Survivor区"锁共享

-XX NewRatio
Young区大小与Old区大小的比值(除去Permanent区)
Xms=Xmx并且设置了Xmn的情况下,该参数不要进行设置(一般很少使用,因为直接减法就得到了)


-XX SurvivorRatio
Eden区与Survivor区的大小比值,默认是8.2个Survivor区与一个Eden区的比值为2:8=>1个Survivor区占整个Young区的1/10
用来存放JVM刚分配的Java对象

所有新创建的对象都放到Eden区.特殊情况才会放到其他区




-Xmn600M 初始堆内存的大小

-XX:PermSize=500M 持久带的内存大小

我应该使用什么垃圾回收器
JVM产生垃圾,选用好的垃圾回收器
垃圾回收机制,应该选择什么

怎么调整JVM内存的使用


最终目标降低FULL GC的频率
会导致程序的暂停

## 2. 监控
## 2-1. JConsole/JVitural
Windows
1. Windows上安装JConsole和Jvirtual
2. JVM服务器上打开对应端口就行了
3. 使用JConsole或者JVitural去连接JMX就好了
```
vim /usr/local/tomcat/bin/cataline.sh
#############################################################################
#!/bin/sh
CATALINA_OPTS="$CATALINA_OPTS
-Dcom.sun.management.jmxremote
-Dcom.sun.management.jmxremote.port=12345
-Dcom.sun.management.jmxremote.authenticate=false
-Dcom.sun.management.jmxremote.ssl=false
-Djava.rmi.server.hostname=10.0.0.7"

#xxxx
#############################################################################

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 重启Tomcat服务
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

netstat -lnutp | grep 12345
#-----------------------------------------------------------------------------
#(Not all processes could be identified, non-owned process info
# will not be shown, you would have to be root to see it all.)
#tcp6       0      0 :::12345                :::*                    LISTEN      7366/java      
#-----------------------------------------------------------------------------

```

### 2-2. Java gateway
生产环境中使用Java Gateway
+ 修改配置文件
```
# Zabbix-server上配置下面的内容
JavaGateway=192.168.3.14
JavaGatewayPort=10052
StartJavaPollers=5 # 启动多少个进程去轮询它
```
### 2-3. cmdline-jmxclient
Java包.
