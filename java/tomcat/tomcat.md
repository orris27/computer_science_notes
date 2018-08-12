## 1. 安装
+ 统一UID
+ 有两种方法都可以访问tomcat:AJP和HTTP(主要)
```
useradd -u 601 tomcat # tomcat需要登录
id tomcat
#-------------------------------------------------------------------------------------------------
# uid=601(tomcat) gid=1002(tomcat) groups=1002(tomcat)
#-------------------------------------------------------------------------------------------------

mkdir /tools
cd /tools

# wget拿不到,所以只能去官网下载
wget http://download.oracle.com/otn-pub/java/jdk/10.0.2+13/19aef61b38124481863b1413dce1855f/jdk-10.0.2_linux-x64_bin.tar.gz
tar zxf jdk-10.0.2_linux-x64_bin.tar.gz

mv jdk-10.0.2 /usr/local/
ln -s /usr/local/jdk-10.0.2/ /usr/local/jdk

cd /tools
wget http://apache.spinellicreations.com/tomcat/tomcat-9/v9.0.10/bin/apache-tomcat-9.0.10.tar.gz
tar zxf apache-tomcat-9.0.10.tar.gz 
mv apache-tomcat-9.0.10 /usr/local/
ln -s /usr/local/apache-tomcat-9.0.10/ /usr/local/tomcat


cd /usr/local 
ll



###################################################
# 设置环境变量
###################################################

# 虚拟机的话可以直接放这里面,一台虚拟机对应一个用途就好了
cat >> /etc/profile <<EOF
export JAVA_HOME=/usr/local/jdk
export PATH=$JAVA_HOME/bin:$JAVA_HOME/jre/bin:$PATH
export CLASSPATH=$CLASSPATH:$JAVA_HOME/lib:$JAVA_HOME/jre/lib:$JAVA_HOME/lib/tools.jar
export TOMCAT_HOME=/usr/local/tomcat
EOF
source /etc/profile

java -version



###################################################
# 
###################################################
chown -R tomcat:tomcat /usr/local/jdk
chown -R tomcat:tomcat /usr/local/tomcat/
su - tomcat

java -version

#tomcat 默认监听8080
/usr/local/tomcat/bin/startup.sh

netstat -lntup | grep 8080

#++++++++++++++++++++++++++++++++++++++++++++++++++++
# 浏览器访问10.0.0.7:8080
#++++++++++++++++++++++++++++++++++++++++++++++++++++

cd /usr/local/tomcat/conf
vim server.xml

cd /usr/local/tomcat/webapps/
# tomcat默认目录,根在ROOT下.一般不改.如果要访问其他文件夹,要`x:8080/dirname`

vim /usr/local/tomcat/conf/tomcat-users.xml
####################################################################################
<role rolename="manager-gui" />
<role rolename="admin-gui" />
<user username="tomcat" password="123456" roles="manager-gui,admin-gui"/>
####################################################################################

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# temp/ work/放临时文件,我们推荐重启的时候清空他们
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

# 关闭的时候写脚本,先kill,30秒后还在的话就kill -9
/usr/local/tomcat/bin/shutdown.sh

# 这个时候还是tomcat身份
cd /usr/local/tomcat/bin/
vim tomcat.sh
################################################
#! /bin/bash

usage () {
    echo "Usage: $0 [start|stop|status]"
}

start () {
    /usr/local/tomcat/bin/startup.sh
}


stop () {
    TPID=$(ps aux | grep java | grep tomcat | grep -v 'grep' | awk '{print $2}')
    kill -9 $TPID
    sleep 5
    
    TSTAT=$(ps aux | grep java | grep tomcat | grep -v 'grep' | awk '{print $2}')
    if [ -z $TSTAT ];then
        echo "tomcat stop"
    else
        kill -9 $TSTAT
    fi
}

status () {
    ps aux | grep java | grep tomcat | grep -v 'grep' 
}

main () {
case "$1" in
	start)
		start
		;;
	stop)
		stop
		;;
	status)
		status
		;;
	*)
		usage
		;;
esac
}
main $1
################################################

chmod +x tomcat.sh
./tomcat.sh start     

#+++++++++++++++++++++++++++++++++++++++++++++++++++++
# 浏览器访问10.0.0.7:8080/host_manager/html
# 输入对应的用户名和密码
#+++++++++++++++++++++++++++++++++++++++++++++++++++++

# 可以用作监控,不用部署Zabbix
#+++++++++++++++++++++++++++++++++++++++++++++++++++++
# 浏览器访问10.0.0.7:8080/manager/status 
# 输入对应的用户名和密码
#+++++++++++++++++++++++++++++++++++++++++++++++++++++



mv host-manager/ /tmp/
mv examples/ /tmp/
mv docs/ /tmp/
```
