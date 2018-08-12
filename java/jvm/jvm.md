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


#### 1-1-1. Young
Young区=1个eden区+2个Survivor区(e+s0+s1)

-Xmn Young区大小 (1.4之前是-XX)
比如配置成100M,那么就被"1个eden区+2个Survivor区"锁共享

-XX NewRatio
Young区大小与Old区大小的比值(除去Permanent区)
Xms=Xmx并且设置了Xmn的情况下,该参数不要进行设置(一般很少使用,因为直接减法就得到了)


-XX SurvivorRatio
Eden区与Survivor区的大小比值,默认是8.2个Survivor区与一个Eden区的比值为2:8=>1个Survivor区占整个Young区的1/10
用来存放JVM刚分配的Java对象

大部分新创建的对象都放到Eden区.下面情况会直接分配到Old区
1. 大对象可以通过设置参数来代表超过多大时就不在Young区分配,而是直接在Old区分配
2. 大的数组对象,切数组中无引用外部对象(数组必须连续,所以会占用很大空间)



#### 1-1-2. Old/Tenured
Old区大小=Heap大小-Young大小-Permanent大小
没有参数配置Old区大小
Young区中经过垃圾回收没有回收掉的对象被复制到Old区,比如说回收了一次后发现又被引用了,就回收不了

Old存储对象比Young年龄大的多,而且不乏大对象


缓存对象一般是弱引用
减少弱引用
缓存数据库一般都有一个初始值.而Java的缓存会伸缩.
Old区如果Java的缓存伸缩占了很多区域,就导致垃圾回收后大对象从Eden区进不了Old区=>Java就会进行Full GC,将他们全部都回收掉


#### 1-1-3. Permanent
Permanent大小=Heap大小-Young大小-Old大小
-XX:PermSize:
-XX:MaxPermSize:
配成相同,原因同理
存放Class,Method元信息,其大小与项目的规模,类,方法的数量有关.一般设置为128M就足够了,设置原则是预留30%的空间
  + Class的版本号,属性值,子类,接口

lib在用的时候加载.加载到Permanent区.从类库中加新的包后,就会继续装到Permanent区.
如果超出Permanent区后,由于Class很重要,所以就会溢出=>所以就只能调大.(我们通过监控查看如果超过大小就调大)
比如static就会放到Permanent区里面
可以被回收
    1. 常量池中的常量 => 没有引用的常量就可以回收(常量的回收最简单)
    2. 无用的类信息
    3. 无用的类,必须保证下面三点才能回收 => 实际很少会去回收
        1. 类的所有实例已经被回收
        2. 加载类的ClassLoader已经被回收
        3. 类对象的Class对象没有被引用(即没有通过反射引用该类的地方)


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
