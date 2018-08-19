    
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


## 2. 配置
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

### 2-2. 完全分布式
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
