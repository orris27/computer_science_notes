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


### 1-2. 内存垃圾回收
#### 1-2-1. 过程
1. 收集 => 被垃圾回收器占用空间
    1. 引用计数算法
        1. 原理
            新增一个引用时计数+1,引用释放时计数-1.计数为0时可以回收
        2. 难题
            1. 精准计数
            2. 无法解决对象相互循环引用的问题
    2. 根搜索算法
        1. 创建一个对象,就在root等节点下挂1个.如果2个对象产生联系,就在这2个节点中连线.然后我们搜索的时候从root开始往下,如果发现孤立的子树,就认为可以垃圾回收
        2. GC Roots
            1. 虚拟机栈中引用的对象
            2. 方法区中类静态属性实体引用的对象
            3. 方法区中常量引用的对象
            4. 本地方法中JNI引用的对象



        
        
2. 回收
    1. 复制算法(Copying)
        1. 使用在Eden=>Survivor0/Survivor1
        2. 原理
            + 从Eden到S0/S1的时候,就是扫遍Eden,然后如果有用的,就复制放到S0/S1(里面肯定有一个是空的)里,没用的就扔了
    2. 标记清除算法(Mark-Sweep)
        1. 会产生碎片
        2. 原理
            + 第一次扫描对存活的对象做标记,第二次扫描后将整个空间中未被标记的对象进行标记
    3. 标记整理压缩算法(Mark-Compac)
        1. 不产生碎片
        2. 慢
        3. 原理
            + 和标记清除算法类似,但是会做移动=>清除碎片+占用时间
            
#### 1-2-2. 概念
1. 串行回收
    + gc单线程内存回收,会暂停所有用户线程
2. 并行回收
    + 指多个GC线程并行工作,但也会暂停所有用户线程
    1. Serial:单行
    2. Paralle收集器:并行
    3. CMS收集器:并发
    
3. 并发回收
    + GC线程与用户线程同时执行.不需要停顿用户线程(CPU的切换)

#### 1-2-3. JVM常见垃圾回收器
> [JVM垃圾回收器图](https://upload-images.jianshu.io/upload_images/1951322-bec12cb02de0f88e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/547)
连线表示组合
Young:
    1. Serial
    2. ParNew
    3. Parallel Scavenge
Old:
    1. CMS
    2. Serial Old(MSC)
    3. Parallel Old

##### 1-2-3-1. 介绍
1. Serial 回收器
    1. Safepoint:对收集的垃圾进行回收的时间点
        1. 多个用户线程=>Safepint=>GC线程(Young采取复制算法进行垃圾回收,暂停所有用户线程)=>Safepoint=>GC线程(Old采用标记-整理算法)
    2. 优势
        1. 简单
        2. 没有多线程交互开销=>反而更加搞笑
        3. Client模式下默认的新生代收集器
    3. 开启
        1. Young:`-XX:+UserSerialGC`
            1. 复制算法
        2. Old:`-XX:+UserSerialGC`
            1. 标记-压缩算法
2. ParNew 回收器
    1. 和Serial 回收器的区别只是GC线程是多个
    2. 优势
        1. 多核CPU下就有优势
    3. 开启
        1. Young:`-XX:+UserParNewGC`
            1. `-XX:ParallelGCThreads`指定线程数,最好与CPU数量相当
            2. Young使用并行回收收集器,而Old使用串行收集器
            3. 复制算法
        2. Young:Parallel Scavenger回收器
            1. 吞吐量:运行用户代码的时间/总时间
                + 总时间=运行用户代码的时间+垃圾收集
            2. 开启:`-XX:+UseParallelGC`
                1. 使用Parallel Scavenge+Serial Old收集器组合回收垃圾
            3. `-XX:GCTimeRatio`
                设置用户执行时间占总时间的比例,默认99,即1%的时间用来进行垃圾回收
                
            4. `-XX:MaxGCPauseMillis`:设置GC的最大停顿时间
            5. 使用复制算法
        3. Old:Parrallel Old回收器
            1. 开启:`-XX:+UseParallelOldGC`
            2. 使用标记-整理算法
            3. 并行的独占式的垃圾回收器
            
            
3. CMS(并发标记清除)回收器
    1. 原理
        1. 初始阶段:值标记GC Roots能直接关联到的对象
        2. 并发标记:进行GC Roots Tracing的过程
        3. 重新标记:修正并标记期间因用户程序继续运行而导致标记发生改变的那一部分对象的标记
        4. 并发清除
    2. 注意:
        1. 标记和重新标记2个阶段仍然需要Stop-The-World.
        2. 并发标记和并发清除过程中可以和用户线程一起工作
        3. 并发标记和并发清除耗时最长
    3. 目前使用最广泛的
    
    4. 算法:标记-清除
    5. `-XX:ParallelCMSThreads`
        + 手工设定CMS的线程数量
        + 默认为(ParallelGCThreads+3)/4
    6. 开启:`-XX:+UseConcMarkSweepGC`
        1. 收集器组合:ParNew+CMS+Serial Old
        2. Serial Old作为CMS出现"Concurrent Mode Failure"失败后的后备收集器使用
    7. `-XX:CMSInitiatingOccupancyFraction`
        1. 设置CMS收集器在Old空间被使用多久后触发垃圾收集
        2. 默认68%,但老师推荐设置为70%
    8. `-XX:UseCMSCompactAtFullCollection`
        1. CMS使用标记-清除算法=>碎片
        2. 这个参数设置:垃圾收集器后是否需要一次内存碎片整理过程,仅在CMS收集器时有效
    9. `-XX:+CMSFullGCBeforeCompaction`
        1. 设置CMS收集器在进行若干次垃圾收集后再进行一次内存碎片整理过程
        2. 常与`UseCMSCompactAtFullCollection`参数一起使用
    10. `-XX:CMSInitiationPermOccupancyFraction`
        1. 设置Perm Gen使用到达多少比率时触发,默认92%

新生代垃圾回收的算法就是复制算法
        
-XX:+UseSerialGC来开启:Serial New+Serial Old的收集器组合进行内存回收


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
