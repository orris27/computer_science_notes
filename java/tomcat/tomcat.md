## 1. 安装
### 1-1. 压缩包安装
1. 创建Tomcat统一用户
    + 统一UID
2. 压缩包安装JRE
3. 压缩包安装Tomcat
4. 设置Java环境变量
5. 使用tomcat用户,启动Tomcat
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
# 使用tomcat用户启动Tomcat
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
```

## 2. Tomcat启动关闭脚本
```
su - tomcat

# 这个时候还是tomcat身份
cd /usr/local/tomcat/bin/
vim tomcat.sh
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 参考https://github.com/orris27/orris/blob/master/java/tomcat/tomcat.sh
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

chmod +x tomcat.sh
./tomcat.sh start     
```


## 3. 管理Tomcat
### 3-1. manager
1. 授权用户
2. 授权其他机器访问
3. 移除不需要的目录

```
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

vim /usr/local/tomcat/webapps/manager/META-INF/context.xml 
#########################################################################################
<Context antiResourceLocking="false" privileged="true" >
  <!-- 注释掉下面的内容
  <Valve className="org.apache.catalina.valves.RemoteAddrValve"
         allow="127\.\d+\.\d+\.\d+|::1|0:0:0:0:0:0:0:1" />
  -->
  <!-- xxxxxxxx -->
</Context>mulu
#########################################################################################


# 关闭的时候写脚本,先kill,30秒后还在的话就kill -9
/usr/local/tomcat/bin/tomcat.sh stop
/usr/local/tomcat/bin/tomcat.sh start # 自己写的脚本,见上面


#+++++++++++++++++++++++++++++++++++++++++++++++++++++
# 浏览器访问10.0.0.7:8080/host_manager/html
# 输入对应的用户名和密码
#+++++++++++++++++++++++++++++++++++++++++++++++++++++

# 可以用作监控,不用部署Zabbix
#+++++++++++++++++++++++++++++++++++++++++++++++++++++
# 浏览器访问10.0.0.7:8080/manager/status 
# 输入对应的用户名和密码
#+++++++++++++++++++++++++++++++++++++++++++++++++++++

cd /usr/local/tomcat/webapps/
mv host-manager/ /tmp/
mv examples/ /tmp/
mv docs/ /tmp/
```

### 3-2. 8005
#### 3-2-1. 使用
```
telnet 127.0.0.1 8005
#####################################
SHUTDOWN
#####################################
```
#### 3-2-2. 优化
修改端口为其他,如8527
```
vim /usr/local/tomcat/conf/server.xml
####################################################################
<Server port="8527" shutdown="dangerous">
####################################################################
```



## 4. 优化
1. 取消8005端口



