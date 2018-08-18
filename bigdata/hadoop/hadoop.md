    
## 1. 安装
```
###################################################################################
# 安装依赖包
###################################################################################
yum install -y wget 




###################################################################################
# 安装JDK
###################################################################################
mkdir /tools
cd /tools

# wget拿不到,所以只能去官网下载
wget http://download.oracle.com/otn-pub/java/jdk/10.0.2+13/19aef61b38124481863b1413dce1855f/jdk-10.0.2_linux-x64_bin.tar.gz
tar zxf jdk-10.0.2_linux-x64_bin.tar.gz

mv jdk-10.0.2 /usr/local/
ln -s /usr/local/jdk-10.0.2/ /usr/local/jdk

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

java -version



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
