    
## 1. 安装(3.1.1版本Hadoop)
```
###################################################################################
# 确认系统环境
###################################################################################
[root@hadoop01 ~]# cat /etc/redhat-release 
CentOS Linux release 7.5.1804 (Core) 
[root@hadoop01 ~]# uname -r
3.10.0-862.el7.x86_64
[root@hadoop01 ~]# uname -m
x86_64







###################################################################################
# 安装依赖包
###################################################################################
yum install -y wget 


###################################################################################
# 关闭防火墙,selinux
###################################################################################
systemctl stop iptables
systemctl disable iptables
systemctl stop firewalld
systemctl disable firewalld
setenforce 0
sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config




###################################################################################
# 安装JDK
###################################################################################
mkdir /tools
cd /tools

# wget拿不到,所以只能去官网下载
wget http://download.oracle.com/otn-pub/java/jdk/8u181-b13/96a7b8442fe848ef90c96a2fad6ed6d1/jdk-8u181-linux-x64.tar.gz
tar zxf jdk-8u181-linux-x64.tar.gz
mv jdk1.8.0_181/ /usr/local/
ln -s /usr/local/jdk1.8.0_181/ /usr/local/jdk

# 设置环境变量
# 虚拟机的话可以直接放这里面,一台虚拟机对应一个用途就好了
vi /etc/profile
###################################################
export JAVA_HOME=/usr/local/jdk
export JRE_HOME=$JAVA_HOME/jre
export CLASSPATH=${CLASSPATH}:${JAVA_HOME}/lib:${JRE_HOME}/lib:${JAVA_HOME}/lib/tools.jar
export PATH=$JAVA_HOME/bin:$JAVA_HOME/jre/bin:$PATH
###################################################
source /etc/profile

java -version # 必须下载1.8.0版本,不能用10.0.0,否则NodeManager等就启动不了!!
#------------------------------------------------------------------------------------
# java version "1.8.0_181"
# Java(TM) SE Runtime Environment (build 1.8.0_181-b13)
# Java HotSpot(TM) 64-Bit Server VM (build 25.181-b13, mixed mode)
#------------------------------------------------------------------------------------



###################################################################################
# 安装Hadoop
###################################################################################
cd ~/tools
wget http://apache.rediris.es/hadoop/common/hadoop-3.1.1/hadoop-3.1.1.tar.gz
tar zxf hadoop-3.1.1.tar.gz
mv hadoop-3.1.1 /usr/local/
ln -s /usr/local/hadoop-3.1.1/ /usr/local/hadoop

vi /etc/profile
###################################################
HADOOP_INSTALL=/usr/local/hadoop
export PATH=$HADOOP_INSTALL/sbin:$HADOOP_INSTALL/bin:$PATH
###################################################
source /etc/profile

hadoop version #出现下面内容就说明Hadoop安装成功了
#---------------------------------------------------------------------------------------
# Hadoop 3.1.1
# Source code repository https://github.com/apache/hadoop -r 2b9a8c1d3a2caf1e733d57f346af3ff0d5ba529c
# Compiled by leftnoteasy on 2018-08-02T04:26Z
# Compiled with protoc 2.5.0
# From source with checksum f76ac55e5b5ff0382a9f7df36a3ca5a0
# This command was run using /usr/local/hadoop-3.1.1/share/hadoop/common/hadoop-common-3.1.1.jar
#---------------------------------------------------------------------------------------

```


## 2. 模式
### 2-1. 独立模式
默认的配置就是独立模式.独立模式没有守护进程
```
jps # 查看Java程序的进程
#---------------------------------------------------------------------------------------
# 1832 Jps
#---------------------------------------------------------------------------------------


hadoop fs -ls / # 会发现就是本机的"/"目录
```
### 2-2. 伪分布模式
1. 节点
    1. 名称节点(nameNode):存放整个文件的目录,但不存放实际文件
    2. 辅助名称节点(secondaryNameNode):对nameNode的备份
    3. 数据节点(dataNode):存放实际文件
2. 特点
    1. 只有1个主机
    2. YARN就是localhost
    3. Hadoop实际上没有区分伪分布式和完全分布式
```
###################################################################################
# 配置Hadoop的伪分布式的配置文件
###################################################################################
cd /usr/local/hadoop/etc/
cp -R hadoop/ hadoop-pseudo/
cd hadoop-pseudo/

vim core-site.xml 
##############################################################################
<configuration>
        <property>
             <name>fs.defaultFS</name>
             <value>hdfs://localhost/</value>
        </property>
</configuration>
##############################################################################


vim hdfs-site.xml 
##############################################################################
<configuration>
        <property>
             <name>dfs.replication</name>
             <value>1</value>
        </property>
</configuration>
##############################################################################


vim yarn-site.xml 
##############################################################################
<configuration>
    <property>
        <name>yarn.resourcemanager.hostname</name>
        <value>localhost</value>
    </property>
    <property>
        <name>yarn.nodemanager.aux-services</name>
        <value>mapreduce_shuffle</value>
    </property>
</configuration>
##############################################################################


vim mapred-site.xml 
##############################################################################
<configuration>
    <property>
        <name>mapreduce.framework.name</name>
        <value>yarn</value>
    </property>
</configuration>
##############################################################################

vim /usr/local/hadoop/etc/hadoop-pseudo/hadoop-env.sh
##############################################################################
export JAVA_HOME=/usr/local/jdk
##############################################################################



###################################################################################
# 允许能ssh到本机
###################################################################################
ssh-keygen -t dsa -P '' -f ~/.ssh/id_dsa
cd ~/id_dsa
cat id_dsa >> authorized_keys
ssh localhost # Hadoop伪分布式必须要求本机能ssh自己本机,而且为了方便,最好不用密码认证
exit


hadoop namenode -format

cd /usr/local/hadoop/etc/

vim /root/.bashrc
##################################################################################
export HDFS_NAMENODE_USER="root"
export HDFS_DATANODE_USER="root"
export HDFS_SECONDARYNAMENODE_USER="root"
export YARN_RESOURCEMANAGER_USER="root"
export YARN_NODEMANAGER_USER="root"
##################################################################################
source /root/.bashrc


# 如果使用start-all脚本的话,就不用再输入dfs和yarn了
#start-all.sh  --config ${HADOOP_INSTALL}/etc/hadoop-pseudo

# 配置目录可以通过环境变量(HADOOP_CONF_DIR)或者--conf来指定
#export HADOOP_CONF_DIR=${HADOOP_INSTALL}/etc/hadoop-pseudo
start-dfs.sh  --config ${HADOOP_INSTALL}/etc/hadoop-pseudo
#-------------------------------------------------------------------------------------
# Starting namenodes on [localhost]
# Last login: Sun Aug 19 06:45:53 CST 2018 from 10.0.0.1 on pts/0
# Starting datanodes
# Last login: Sun Aug 19 06:46:02 CST 2018 on pts/0
# Starting secondary namenodes [hadoop]
# Last login: Sun Aug 19 06:46:05 CST 2018 on pts/0
#-------------------------------------------------------------------------------------


start-yarn.sh  --config ${HADOOP_INSTALL}/etc/hadoop-pseudo
#-------------------------------------------------------------------------------------
# Starting resourcemanager
# Last login: Sun Aug 19 06:46:15 CST 2018 on pts/0
# Starting nodemanagers
# Last login: Sun Aug 19 06:47:09 CST 2018 on pts/0
#-------------------------------------------------------------------------------------

jps # 我这边还缺少ResourceManager和NodeManager??
#-------------------------------------------------------------------------------------
# 9616 DataNode
# 10100 ResourceManager
# 9479 NameNode
# 10234 NodeManager
# 9852 SecondaryNameNode
# 10636 Jps
#-------------------------------------------------------------------------------------

# 停止服务
#stop-yarn.sh  --config ${HADOOP_INSTALL}/etc/hadoop-pseudo
#stop-dfs.sh  --config ${HADOOP_INSTALL}/etc/hadoop-pseudo

######################################################################
# 浏览器访问8088,50070
######################################################################

# 停止服务
stop-yarn.sh  --config ${HADOOP_INSTALL}/etc/hadoop-pseudo
stop-dfs.sh  --config ${HADOOP_INSTALL}/etc/hadoop-pseudo


jps

start-all.sh  --config ${HADOOP_INSTALL}/etc/hadoop-pseudo


hadoop fs -ls / # 发现还是原来我们系统的/目录,因为还是需要指定配置目录
export HADOOP_CONF_DIR=${HADOOP_INSTALL}/etc/hadoop-pseudo
hadoop fs -ls / # 发现里面没有任何东西
hadoop fs -mkdir /usr/
hadoop fs -ls /
#------------------------------------------------------------------------
# Found 1 items
# drwxr-xr-x   - root supergroup          0 2018-08-19 07:29 /usr
#------------------------------------------------------------------------
``` 

### 2-3. 完全分布式
1. 环境准备
    1. 节点
        1. 节点1
            1. 主机名:hadoop01
            2. ip:`10.0.0.7`
        2. 节点2
            1. 主机名:hadoop02
            2. ip:`10.0.0.8`
        3. 节点3
            1. 主机名:hadoop03
            2. ip:`10.0.0.9`
        4. 节点4
            1. 主机名:hadoop04
            2. ip:`10.0.0.10`
    2. 能自己ssh自己
2. 修改配置文件,将完全分布式的配置文件推送到其他节点上,保证一致
3. 删除原有的Hadoop文件系统,并重新格式化
4. 在NameNode上启动Hadoop
```
###########################################################################################
# 使用软连接解决配置目录的问题
###########################################################################################
cd /usr/local/hadoop/etc/
cp -R hadoop hadoop-local
rm -rf hadoop
cp -R hadoop-pseudo/ hadoop-fully
ln -s /usr/local/hadoop/etc/hadoop-fully/ /usr/local/hadoop/etc/hadoop


#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 克隆出三台hadoop02,hadoop03,hadoop04
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#================================================================================
# 4台Hadoop
#================================================================================
###########################################################################################
# 域名解析+不用密码ssh
###########################################################################################
ssh localhost

cat >> /etc/hosts <<EOF
10.0.0.7 hadoop01
10.0.0.8 hadoop02
10.0.0.9 hadoop03
10.0.0.10 hadoop04
EOF 
ssh hadoop01
ssh hadoop02
ssh hadoop03
ssh hadoop04

#================================================================================
# hadoop01
#================================================================================
###########################################################################################
# 修改配置文件
###########################################################################################
cd /usr/local/hadoop/etc/hadoop-fully/
vim core-site.xml 
######################################################################################
<configuration>
        <property>
             <name>fs.defaultFS</name>
             <value>hdfs://hadoop01/</value>
        </property>
</configuration>
######################################################################################




vim hdfs-site.xml 
######################################################################################
<configuration>
        <property>
             <name>dfs.replication</name>
             <value>2</value>
        </property>
</configuration>
######################################################################################



vim yarn-site.xml 
######################################################################################
<configuration>
    <property>
        <name>yarn.resourcemanager.hostname</name>
        <value>hadoop01</value>
    </property>
    <property>
        <name>yarn.nodemanager.aux-services</name>
        <value>mapreduce_shuffle</value>
    </property>
</configuration>
######################################################################################


vim workers
######################################################################################
hadoop02
hadoop03
######################################################################################

cd $HADOOP_INSTALL/etc
scp -r hadoop-fully/ root@hadoop02:/usr/local/hadoop/etc/
scp -r hadoop-fully/ root@hadoop03:/usr/local/hadoop/etc/
scp -r hadoop-fully/ root@hadoop04:/usr/local/hadoop/etc/
ssh hadoop02 cat /usr/local/hadoop/etc/hadoop-fully/core-site.xml 
ssh hadoop03 cat /usr/local/hadoop/etc/hadoop-fully/core-site.xml 
ssh hadoop04 cat /usr/local/hadoop/etc/hadoop-fully/core-site.xml 








###########################################################################################
# 对所有节点格式化
###########################################################################################
ssh hadoop01 rm -rf /tmp/hadoop-root/*
ssh hadoop02 rm -rf /tmp/hadoop-root/*
ssh hadoop03 rm -rf /tmp/hadoop-root/*
ssh hadoop04 rm -rf /tmp/hadoop-root/* # 这里其实hadoop04没必要格式化

ssh hadoop01 /usr/local/hadoop/bin/hdfs namenode -format 
ssh hadoop02 /usr/local/hadoop/bin/hdfs namenode -format 
ssh hadoop03 /usr/local/hadoop/bin/hdfs namenode -format 
ssh hadoop04 /usr/local/hadoop/bin/hdfs namenode -format 










###########################################################################################
# 启动服务
###########################################################################################
start-all.sh 














###########################################################################################
# 验证
###########################################################################################
jps
#---------------------------------------------------------------------------------------
# 14113 Jps
# 13522 SecondaryNameNode
# 13245 NameNode
# 13774 ResourceManager
#---------------------------------------------------------------------------------------


ssh hadoop02 /usr/local/jdk/bin/jps 
#---------------------------------------------------------------------------------------
# 4736 NodeManager
# 4599 DataNode
# 4888 Jps
#---------------------------------------------------------------------------------------


ssh hadoop03 /usr/local/jdk/bin/jps 
#---------------------------------------------------------------------------------------
# 3746 Jps
# 3652 NodeManager
# 3542 DataNode
#---------------------------------------------------------------------------------------

```
## 3. 操作
```
hadoop fs -mkdir -p /user/it18zhang/
hadoop fs -ls -R /

```

## 4. 启动脚本
### 4-1. start-all.sh
#### 4-1-1. 总结
1. 调用hadoop-config.cmd
    1. 调用hadoop-env.cmd
        1. 设置JAVA_HOME,HADOOP_CLASSPATH,和Hadoop各种node的参数
2. 调用start-dfs.cmd
    1. 调用hadoop-config.cmd
    2. `hadoop namenode`
        1. 调用hadoop-config.cmd
        2. 调用hdfs.cmd
            1. 调用mapred-config.cmd
                1. 调用hadoop-config.cmd
            2. 调用mapred-env.cmd
        3. 执行Java程序
    3. `hadoop datanode`
3. 调用start-yarn.cmd
    1. 调用yarn-config.cmd
        1. 调用hadoop-config.cmd
    2. `yarn resourcemanager`
        1. 调用yarn-config.cmd
        2. 调用yarn-env.cmd
            1. 设置yarn用户
            2. 设置yarn的配置目录
            3. 设置Java的最大堆内存为yarn的堆内存大小
            4. 设置yarn的日志目录
            5. 设置yarn的日志文件名称
            6. 设置yarn的policy文件
            7. 设置yarn的root loger
            8. 设置yarn的选项
            9. 添加Java类库的路径到yarn的选项
            10. 添加policy文件到yarn的选项
        3. 执行Java程序
    3. `yarn nodemanager`
    4. `yarn proxymanager`
    
    
#### 4-1-2. Windows的旧版Hadoop
1. 推荐我们不要用这个脚本,而是dfs和yarn
2. 设置环境变量
    #1. 设置HADDOP_BIN_PATH变量为`${HADOOP_INSTALL}/sbin`
    2. 设置DEFAULT_LIBEXEC_DIR变量,为`${HADOOP_HOME}/libexec`
3. 调用`libexec/hadoop-config.cmd`
    
    1. 判断有没有定义common_dir,在hadoop的share/hadoop下有common;
    2. 类似地,提取common,common/lib,common/lib/native,hdfs,yarn等目录
    3. 把Hadoop里的所有jar包都提取出来
    4. 单独判断`hadoop-common-*.jar`文件如果没有的话,就会说JAVA_HOME没有设置或找不到
    5. 设置HADOOP_CONF_DIR为`${HADOOP_INSTALL/etc/hadoop}`,用在启动脚本里面的`--config`参数
    6. 如果在命令行中自己指定`--config`的话,就将HADOOP_CONF_DIR设置为命令行中指定的目录
    7. 如果在命令行中自己指定`--hosts`的话,就引用里面的数据节点的列表
    8. 如果config目录下有hadoop-env.cmd的话,就调用这个
        1. 设置`JAVA_HOME`变量
        2. 设置`HADOOP_CLASSPATH`
        3. 设置Hadoop名称节点,数据节点,辅助名称节点,客户端的选项
    9. 如果没定义`JAVA_HOME`就抛出错误
    10. 如果`$JAVA_HOME/bin/java.exe`不存在的话,也抛出错误
    11. 设置JAVA的变量为`$JAVA_HOME/bin/java`
    12. 设置JVM的参数
        1. `-Xmx10000m`:最大堆内存为1000m
    13. 如果定义了Hadoop的堆,HADOOP_HEAPSIZE,那么就会覆盖Java的最大对内存的值
    14. 大量设置类路径
        1. 设置CLASS_PATH为Hadoop的配置文件目录
        2. 如果定义了Hadoop的类路径,就添加进来
    15. 定义日志
    16. for循环取出用java来执行类路径变量的所有路径的内容,外加JVM的参数,并设置Java平台
    17. 设置Java的类库路径
    18. 在Hadoop下有个tools/lib文件夹,
    19. 定义Java的类路径
    20. 使用Ipv4的版本
    21. 如果没有定义HADOOP_HDFS_HOME,就设置为HADOOP_HOME
    22. 设置CLASSPATH,取出hdfsdir下的所有
    23. 设置yarn的目录变量等
    24. 设置mapreduce的目录变量
    
4. 调用sbin/start-dfs.sh
    1. 如果没有定义HADOOP_BIN_PATH的话,就设置为${HADOOP_INSTALL}/sbin
    2. 定义执行类库的路径,libexec
    3. 调用`hadoop-config.cmd`,并且把start-dfs的命令行参数作为其命令行参数
    4. 判断有没有在命令行中自定义`--config`
    5. `start "Apache Hadoop Distribution" hadoop namenode`
        1. start是windows里启动一个新窗口并启动一个新程序,引号内是这个窗口的标题
        2. `hadoop namenode`是在命令行中的命令.hadoop实际上是`hadoop.cmd`
            1. 如果没有定义HADOOP_BIN_PATH的话,就设置为${HADOOP_INSTALL}/sbin
            2. 设置一系列path的变量,并且替换空格等
            3. for循环添加到现存的路径
            4. 设置libexec的变量
            5. 调用`hadoop-config.cmd`
                1. 参考前面的...
            6. 提取第一个参数,为hadoop-command,就是namenode或者datanode这个字符串
            7. 处理命令参数,最终设置hadoop-command-arguments
                1. 如果是--config就移动
                2. 循环给_arguments赋值
            8. 定义hdfs的命令组
            9. 判断bin目录下hdfs.cmd是否存在,如果有就调用
                1. 定义Hadoop的可执行路径
                2. 定义执行类库的路径,libexec
                3. 调用libexec下的mapred-config.cmd
                    1. 如果没有定义HADOOP_BIN_PATH的话,就设置为${HADOOP_INSTALL}/sbin
                    2. 设置libexec路径
                    3. 如果libexec下存在hadoop-config.cmd文件的话,就调用
                        1. 可以参考前面的分析.定义HADOOP_COMMON_DIR变量,start-all也调用了
                        2. 主要就是做环境变量的参数
                4. 如果存在mapred-env.cmd文件的话,就调用          
                    1. 设置`HADOOP_JOB_HISTORYSERVER_HEAPSIZE=1000`变量
                    2. 设置`HADOOP_MAPRED_ROOT_LOGER=INFO,RFA`变量
                5. 将第一个参数取出来作为hdfs-command
                6. 循环取出参数,放到hdfs-command-arguments
                7. 如果输入错误就打印Usage
                8. 调用hdfs-commands里的命令标签,包括namenode,secondarynamenode,datanode
                9. 每个hdfs-commands里的命令都设置CLASS变量,对应各自的Java类
                10. 设置JVM参数
                11. 调用Java程序去执行
                
            10. 设置MapReduce的命令组,包括pipes,
            11. 判断mared.cmd是否存在,如果有就调用
                1. 定义Hadoop的可执行路径
                2. 定义执行类库的路径,libexec
                3. 调用libexec下的mapred-config.cmd
                    1. 如果没有定义HADOOP_BIN_PATH的话,就设置为${HADOOP_INSTALL}/sbin
                    2. 设置libexec路径
                    3. 如果libexec下存在hadoop-config.cmd文件的话,就调用
                        1. 参考上面的
                4. 如果存在mapred-env.cmd文件的话,就调用          
                    1. 
                5. 将第一个参数取出来作为mapred-command
                6. 循环取出参数,放到mapred-command-arguments
                7. 如果输入错误就打印Usage
                8. 调用mapred-commands里的命令标签,包括classpath,job,queue,sampler,pipes等
                9. 每个mapred-commands里的命令都设置CLASS变量,对应各自的Java类
                10. 设置JVM参数
                11. 调用Java程序去执行
                
            12. 如果hadoop-command等于classpath的话,就输出类路径.(classpath不是hdfs命令,也不是MapReduce命令)
            13. 设置corecommands这个核心命令组,包括`fs`,`version`,`jar`等
            14. 在hadoop-commands中找出核心命令组,将他们标记为核心命令组,即`set corecommand=true`
            15. 如果hadoop-commnad命令组里的命令是核心命令的话,就调用hadoop-command的命令标签,否则设置变量CLASSPATH,CLASS
                1. fs
                    1. 定义类的环境变量CLASS为对应Java的类的位置
                2. version
                    1. 同上
                3. checknative
                4. distcp
                5. daemonlog
                6. archive
                7. 等等
            16. 设置操作系统的path变量
            17. 设置Hadoop的选项HADOOP_OPTS
            18. 把命令名称作为CLASS值
            19. 调用Java程序执行不是核心命令组的hadoop-commands,使用Hadoop的选项和CLASS变量作为参数
            
            
            
        3. hdfs.cmd
            1. 参考上面的
        
    6. `start "Apache Hadoop Distribution" hadoop datanode`
        1. start是windows里启动一个新窗口并启动一个新程序
        2. `hadoop datanode`是在命令行中的命令.hadoop实际上是`hadoop.cmd`
            1. 参考上面的`hadoop namenode`
5. 调用sbin/start-yarn.sh
    1. 设置可执行目录
    2. 设置可执行的libexec目录
    3. 调用yarn-config.cmd
        1. 设置Hadoop可执行目录
        2. 设置libexec目录
        3. 调用hadoop-config.cmd

        4. 如果有--config参数的话,就设置为YARN_CONF_DIR变量,否则就是"$HADOOP_YARN_HOME"/conf目录
        5. 如果有--hosts参数的话,就设置为YARN_SLAVES.
    4. `start "Apache Hadoop Distribution" yarn resourcemanager`
        1. 设置Hadoop可执行目录
        2. 设置libexec目录
        3. 调用yarn-config.cmd
        4. 调用yarn-env.cmd
            1. 设置yarn用户
            2. 设置yarn的配置目录
            3. 设置Java的最大堆内存为yarn的堆内存大小
            4. 设置yarn的日志目录
            5. 设置yarn的日志文件名称
            6. 设置yarn的policy文件
            7. 设置yarn的root loger
            8. 设置yarn的选项
            9. 添加Java类库的路径到yarn的选项
            10. 添加policy文件到yarn的选项
        5. 设置yarn的命令为第一个参数
        6. 设置命令行参数组,附加到yarn的命令行参数组里
        7. 打印Usage,如果没有给yarn参数
        8. 设置Java的最大堆内存为yarn的堆内存大小
        9. assert Hadoop的配置目录必须定义
        10. 设置CLASSPATH
        11. 判断一系列HADOOP_YARN_HOME(在hadoop-config文件里,将yarn的家目录设置为Java的家目录)
        12. 添加yarn的家目录,yarn的dir,yarn的lib_jars_dir(也是在hadoop-config里设置的)到CLASSPATH
        13. 调用yarn命令的标签(设置CLASS+YARN_OPTS)
        14. 设置yarn的选项
        15. 设置JVM参数
        16. 调用Java
        
        
    5. `start "Apache Hadoop Distribution" yarn nodemanager`
        1. 参考上面的
    6. `start "Apache Hadoop Distribution" yarn proxymanager`
        1. 参考上面的

#### 4-1-3. Linux Hadoop-3.1.1
hadoop-config.sh内容如下:
1. 导入hadoop-function.sh里的函数
2. 执行hadoop-bootstrap,设置hdfs目录,yarn目录,hdfs的java目录,yarn的java目录等环境变量
    ```
    HADOOP_DEFAULT_PREFIX=$(cd -P -- "${HADOOP_LIBEXEC_DIR}/.." >/dev/null && pwd -P)
    HADOOP_HOME=${HADOOP_HOME:-$HADOOP_DEFAULT_PREFIX}
    export HADOOP_HOME
    HADOOP_COMMON_DIR=${HADOOP_COMMON_DIR:-"share/hadoop/common"}
    HADOOP_COMMON_LIB_JARS_DIR=${HADOOP_COMMON_LIB_JARS_DIR:-"share/hadoop/common/lib"}
    HADOOP_COMMON_LIB_NATIVE_DIR=${HADOOP_COMMON_LIB_NATIVE_DIR:-"lib/native"}
    HDFS_DIR=${HDFS_DIR:-"share/hadoop/hdfs"}
    HDFS_LIB_JARS_DIR=${HDFS_LIB_JARS_DIR:-"share/hadoop/hdfs/lib"}
    YARN_DIR=${YARN_DIR:-"share/hadoop/yarn"}
    YARN_LIB_JARS_DIR=${YARN_LIB_JARS_DIR:-"share/hadoop/yarn/lib"}
    MAPRED_DIR=${MAPRED_DIR:-"share/hadoop/mapreduce"}
    MAPRED_LIB_JARS_DIR=${MAPRED_LIB_JARS_DIR:-"share/hadoop/mapreduce/lib"}
    HADOOP_TOOLS_HOME=${HADOOP_TOOLS_HOME:-${HADOOP_HOME}}
    HADOOP_TOOLS_DIR=${HADOOP_TOOLS_DIR:-"share/hadoop/tools"}
    HADOOP_TOOLS_LIB_JARS_DIR=${HADOOP_TOOLS_LIB_JARS_DIR:-"${HADOOP_TOOLS_DIR}/lib"}
    HADOOP_SUBCMD_SUPPORTDAEMONIZATION=false
    HADOOP_REEXECED_CMD=false
    HADOOP_SUBCMD_SECURESERVICE=false
    ```
3. 设置用户的参数变量,保存用户的命令行参数
4. 执行hadoop_find_confdir
    1. 设置hadoop的配置文件所在目录的变量(因为可能是`etc/hadoop/`,也可能是`conf/`)
5. 执行hadoop_exec_hadoopenv
    1. 调用hadoop-env.sh
6. 执行hadoop_import_shellprofiles
7. 执行hadoop_exec_userfuncs
    1. 调用hadoop-user-functions.sh
8. 执行hadoop_exec_user_hadoopenv    
    1. 调用家目录下的.hadoop-env,即用户设置的hadoop环境变量
9. 执行hadoop_verify_confdir
10. 检查被启用的环境变量,如果有的话,就转化成为新的环境变量
    1. `HADOOP_SLAVES`=>`HADOOP_WORKERS`
    2. `HADOOP_SLAVE_NAMES`=>`HADOOP_WORKER_NAMES`
    3. `HADOOP_SLAVE_SLEEP`=>`HADOOP_WORKER_SLEEP`
11. 单独判断Java环境是否建立好了
    1. 是否设置了JAVA_HOME环境变量
    2. JAVA_HOME是否是目录
    3. `$JAVA_HOME/bin/java`是否可执行
12. 基础的初始化
    1. 设置Hadoop的common_home
    2. 设置policy文件,为`hadoop-policy.xml`
    3. 设置hdfs,yarn,MapReduce的家目录
    4. 设置用户,日志,pid,ssh等环境变量指向各自的文件或值
13. 添加native目录到java的libpath
14. 添加到classpath
15. 调用家目录下的.hadooprc文件
    

## 5. 编程
### 5-1. 测试是否能导入MapReduce库
```
cat > Test.java <<EOF
import java.io.IOException;
import org.apache.hadoop.mapreduce.Mapper;

public class Test
{
    public static void main(String[] args)
    {
        System.out.println("Test");
        return;
    }
}
EOF

javac -classpath $(hadoop classpath) Test.java
java Test
```
### 5-2. 温度
> [数据来源](ftp://ftp.ncdc.noaa.gov/pub/data/noaa/1901/)
#### 5-2-1. 网上找到的版本(执行成功)
[使用文档](https://github.com/orris27/orris/blob/master/bigdata/hadoop/codes/MaxTemperature/readme.md)

#### 5-2-2. 老师的版本(执行是执行了,但是outofrange)
1. 编写Map类
2. 编写Reduce类
3. 编写App类
4. 打包编译文件.将上述3个java文件生成jar文件
5. 设置Hadoop的CLASSPATH
6. 执行Hadoop
    1. 温度的数据文件为1901.gz和1902.gz
    2. 输出到不存在的目录里
```
vim MaxTemperature.java
###############################################################################################
import java.io.IOException;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class MaxTemperature {
    public static void main(String[] args) throws Exception {
        Job job = new Job();
        job.setJarByClass(MaxTemperature.class);
        job.setJobName("Max temperature");
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));
        job.setMapperClass(MaxTemperatureMapper.class);
        job.setReducerClass(MaxTemperatureReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);
        job.waitForCompletion(true);
    }
}
###############################################################################################






vim MaxTemperatureMapper.java
###############################################################################################
import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

public class MaxTemperatureMapper extends Mapper<LongWritable, Text, Text, IntWritable>{
    private static final int MISSING = 9999;
    public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
        String line = value.toString();
        String year = line.substring(15, 19);
        int airTemperature;
        if(line.charAt(87) == '+') {
            airTemperature = Integer.parseInt(line.substring(88, 92));
        } else {
            airTemperature = Integer.parseInt(line.substring(87, 92));
        }
        String quality = line.substring(92, 93);
        if(airTemperature != MISSING && quality.matches("[01459]")) {
            context.write(new Text(year), new IntWritable(airTemperature));
        }
    }
}
###############################################################################################



vim MaxTemperatureReducer.java
###############################################################################################
import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class MaxTemperatureReducer extends Reducer<Text, IntWritable, Text, IntWritable> {
    public void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
        int maxValue = Integer.MIN_VALUE;
        for(IntWritable value : values) {
            maxValue = Math.max(maxValue, value.get());
        }
        context.write(key, new IntWritable(maxValue));
    }
}
###############################################################################################



javac -classpath $(hadoop classpath) *.java


jar cvf ./MaxTemperature.jar ./*.class

mkdir /usr/local/hadoop/data
mkdir /usr/local/hadoop/data/in
vim /usr/local/hadoop/data/in/sample.txt
###############################################################################################
0067011990999991950051507004888888889999999N9+00001+9999999999999999999999
0067011990999991950051512004888888889999999N9+00221+9999999999999999999999
0067011990999991950051518004888888889999999N9-00111+9999999999999999999999
0067011990999991949032412004888888889999999N9+01111+9999999999999999999999
0067011990999991950032418004888888880500001N9+00001+9999999999999999999999
0067011990999991950051507004888888880500001N9+00781+9999999999999999999999
###############################################################################################

cd /usr/local/hadoop-1.1.2
hadoop jar MaxTemperature.jar MaxTemperature /usr/local/hadoop/data/in/sample.txt  /usr/local/hadoop/data/out
```



## 6. 参数配置
### 6-1. 帮助
```
cd /usr/local/hadoop/
find . -name 'core-default.xml'
#-------------------------------------------------------------------------------
# ./share/doc/hadoop/hadoop-project-dist/hadoop-common/core-default.xml
#-------------------------------------------------------------------------------

find . -name 'hdfs-default.xml'
#-------------------------------------------------------------------------------
# ./share/doc/hadoop/hadoop-project-dist/hadoop-hdfs/hdfs-default.xml
#-------------------------------------------------------------------------------

find . -name 'mapred-default.xml'
#-------------------------------------------------------------------------------
# ./share/doc/hadoop/hadoop-mapreduce-client/hadoop-mapreduce-client-core/mapred-default.xml
#-------------------------------------------------------------------------------


find . -name 'yarn-default.xml'
#-------------------------------------------------------------------------------
# ./share/doc/hadoop/hadoop-yarn/hadoop-yarn2018-08-20 06:19:37,988 INFO mapreduce.JobSubmitter: number of splits:967-common/yarn-default.xml
#-------------------------------------------------------------------------------

```
### 6-2. 修改参数
1. 修改参数
2. 关闭Hadoop
3. 同步配置文件
4. 删除原来的数据目录
5. 格式化
6. 启动Hadoop

### 6-3. 常用参数
1. 块大小
    1. 作用:如果文件小的话,设置更小的块大小可以提高效率
    2. 修改
        1. `hdfs-site.xml`的`dfs.blocksize`参数
        2. 程序动态修改(如果大于块大小的话,就会选择参数,而不是程序里设置的块大小)
        ```
        vim JobMain.java
        ###################################################################################
        Configuration configuration = new Configuration();
        configuration.setLong(FileInputFormat.SPLIT_MAXSIZE,1024*5); # 这里表示5k的大小进行分割
        Job job = new Job(configuration,"max_temp_job");
        ###################################################################################

        # 最后结果就是如下所示,切割成967份了
        #-------------------------------------------------------------------------
        # 2018-08-20 06:19:37,988 INFO mapreduce.JobSubmitter: number of splits:967
        #-------------------------------------------------------------------------
        ```
    3. 其他
        1. HDFS要求一定要大于1M
2. 存储位置
    1. 作用:比如从/tmp目录更改到其他存储位置
    2. 修改位置:`core-site.xml`的`hadoop.tmp.dir`参数

3. 副本数量
    1. 作用:副本数量为多少,存储1个block的节点数量就多少.(我感觉这里其实不止副本数量,严格来说应该是副本数量+1)
    2. 修改位置:`hdfs-site.xml`的`dfs.replication`参数

    
## 7 .Web
### 7-1. 资源管理器
1. 端口:8088
2. 作用   
    1. 查看日志
    2. 访问DataNode的管理器
    
### 7-2. node管理器
1. 端口:8042



## 8. 对象
### 8-1. job
1. set必须在submit之前使用
2. 推荐使用job.getInstance(),会生成JobConf
3. 允许user配置,提交,控制执行,查询状态
### 8-2. ResourceManager
