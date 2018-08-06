## 1. 安装
1. 直接yum安装
```
yum install -y docker
systemctl start docker
systemctl status docker
```


## 2. 镜像管理
1. 从DockerHub获得CentOS的镜像
```
docker pull centos
docker images
```


### 2-1. 镜像操作
1. 导出镜像
```
docker save centos > /opt/centos.tar.gz
```
2. 导入镜像
```
docker load < /opt/centos.tar.gz
```
3. 删除镜像
+ 有容器了的镜像就不能被删除
```
docker rmi <image_id>
```


## 3. 容器
### 3-1. `docker run`
1. `-t`:分配一个伪终端
2. `--name`:
3. `-i`:保持打开的状态,`-it`一起执行
4. `-d`:让程序已启动就在后台运行

### 3-2. `docker commit`
1. `-m`:message

### 3-3. `nsenter`
1. `-m`:挂载namespace
2. `-n`:进入网络的namespace
3. `-p`:进入pid的namespace
### 3-4. 启动容器
1. 创建并启动容器
+ 镜像:`完整路径:标签`(默认路径在`docker.io`,默认标签是`latest`)
+ 执行的命令
+ 容器有名称+ID=>容器的主机名=容器的ID
+ 容器的名称如果不指定会自动生成
```
docker run centos /bin/echo "hello world"
docker run --rm centos /bin/echo "hello world" # 执行结束后会自动删除容器=>防止我们启动太多容器占用资源
docker run --name mydocker -t -i centos /bin/bash # 启动时指定容器名称
```


2. 使用存在的容器启动
+ 只有启动容器时指定了`-it`的才能用start,否则还是处于exit状态
```
docker start <docker_id>
```

3. 查看帮助
```
docker run --help
```


5. 删除容器
    1. 删除停止的容器
    ```
    docker rm <docker_id>/<docker_name>
    ```
    2. 删除运行中的容器(不推荐)
    ```
    docker rm -f <docker_id>/<docker_name>
    ```
    3. 删除所有的容器(生产中不推荐)
    ```
    docker kill $(docker ps -a)
    ```


6. 容器的状态
+ `-a`:和`virsh list --all`一样,会显示所有包括非活跃状态的容器
+ `-q`:只查看容器的id
```
docker ps -a
docker ps -a -q
```

7. 退出容器
+ 是否停止容器根据进入方式而区别
```
exit
```

8. 进入容器
  1. exit会终止容器式进入容器
  ```
  docker attach <docker_id>
  ```
  2. exit不会终止容器式进入容器
  + 这样真的是进入容器了吗?我感觉好像如果修改里面的文件会直接影响到我的物理机?
  + 我觉得还是`docker exec -it <docker_name> /bin/bash`比较好?
  ```
  yum install util-linux  # 如果没有nsenter的话就需要安装这个包
  docker start <docker_id>
  docker inspect --format "{{.State.Pid}}" <docker_id> # 获得容器的pid
  nsenter --help# 进入到一个进程的命名空间
  nsenter -t <pid> -u -i -n -p # <pid>为前面获得的容器的pid
  
  
  # 可以写成脚本,直接`./ns.sh <docker_id>`就能进入了(推荐)
  ###############################################################
  #! /bin/bash
  PID=$(docker inspect --format "{{.State.Pid}}" $1)
  nsenter -t $PID -u -i -n -p
  ###############################################################
  ```
  

## 4. 启用一个Nginx的容器
+ 默认帮我们做一个NAT的操作,docker0的桥接网卡
+ 使用的Nginx是从本地/DockerHub上的
```
iptables -t nat -vnL
docker run -d -P nginx # 随机映射,回容器的id
docker run -d -p 81:80 nginx # 指定端口映射,返回容器的id
```
### 4-1. 端口映射
1. 随机映射:`-P` => 不会冲突
2. 指定端口映射:`-p`
+ `-p hostPort:containerPort[:udp]`物理机的端口:对应容器里的端口

### 4-2. 查看容器的Nginx信息
+ "nginx -g 'daemon off'" => 放在前台运行,因为nginx会fork进程
+ 把随机端口32769映射到80端口 => 可以访问`http://192.168.56.10:32769`(我的虚拟机ip为192.168.56.10)
```
docker ps 
###################################################################################################
CONTAINER ID  IMAGE   COMMAND                CREATED          STATUS        PORTS                   NAMES
1156d0b1257a  nginx   "nginx -g 'daemon ..." 4 seconds ago    Up 3 seconds  0.0.0.0:32768->80/tcp   distracted_goldberg
###################################################################################################
```

### 4-3. 查看容器Nginx的日志信息
```
docker logs <docker_id>
```


## 5. 管理数据卷
一个特殊的容器,存储数据.其他容器可以从这个容器里获取数据.类似于NFS
### 5-1. 创建并启动数据卷容器
#### 5-1-1. 使用系统默认的物理位置
+ 物理机某目录<=>数据卷容器
+ 指定容器自己的`/data`为数据卷
```
docker run -it --name volume-test1 -v /data centos # 将容器的/data目录用作类似NFS的工作
docker run -d --name nfs -v /data centos # 不进去
```
#### 5-1-2. 使用指定的物理位置
+ 容器里的/opt<=>物理机的/opt
+ 有利于开发
+ 可以设置权限
```
docker run -it --name volume-test1 -v /opt:/opt centos 
docker run -it --name volume-test1 -v /opt:/opt:ro centos # 只读
docker run -it --name volume-test1 -v /opt:/opt:rw centos # 读写
```
#### 5-1-3. 只挂载单shi个文件(很少用)
```
docker run -it --name volume-test1 -v ~/.bash_history:.bash_history centos 
```



### 5-2. 创建一个使用该数据卷的容器
+ 新容器的/data<=>指定数据卷容器的/data
+ 使用上面容器的/data卷
+ 指定数据卷的容器无论运行中还是停止状态,都可以访问
```
docker run -it --name test1 --volumes-from nfs centos
```

### 5-3. 创建数据卷容器时,数据卷在物理机上的位置
`docker inspect <docker_id>`下的"Mounts"下的"Sources"里面,容器里的数据卷,如上面操作中的`/data`<=>物理机下的Mounts>Sources


## 6. 制作镜像
容器=>镜像=>容器=>镜像
### 6-1. 手动制作
1. 先做一个容器,然后在里面安装nginx
2. 执行commit,将这个容器做成镜像(此时还不能直接用,因为这个nginx是后台运行的)
3. 使用第一个版本的镜像启动容器
4. 然后在这个容器里将nginx做成前端运行,再提交成为第二个版本的镜像
5. 利用这第二版本的镜像做一个容器,这个容器就能直接执行nginx命令

```
docker run --name mynginx -it centos
####################################################################################
rpm -ivh https://mirrors.aliyun.com/epel/epel-release-latest-7.noarch.rpm
yum install -y nginx
exit
####################################################################################

dcoker ps -a
#-----------------------------------------------------------------------------------
CONTAINER ID IMAGE  COMMAND      CREATED             STATUS                          PORTS    NAMES
ad756cbf64f0 centos "/bin/bash"  11 minutes ago      Exited (0) About a minute ago            mynginx
#-----------------------------------------------------------------------------------

# 这里的docker_id是ad756cbf64f0
docker commit -m "mynginx" <docker_id> oldboyedu/mynginx:v1 # 打个标签v1,名字是mynginx,用户名就是oldboyedu

docker images
#-----------------------------------------------------------------------------------
[root@docker ~]# docker images
REPOSITORY              TAG                 IMAGE ID            CREATED             SIZE
oldboyedu/orris_nginx   v1                  a6657a4c23ff        3 seconds ago       401 MB
docker.io/nginx         latest              c82521676580        12 days ago         109 MB
docker.io/centos        latest              49f7960eb7e4        2 months ago        200 MB
#-----------------------------------------------------------------------------------


# 直接写镜像名字会从docker.io/或者网络里面找,所以要加路径
# docker run -it --name nginxv1 mynginx:v1 # v1标签,另外是latest,默认是latest
docker run -it --name nginxv1 oldboyedu/mynginx:v1 # v1标签,另外是latest,默认是latest
#######################################################
vi /etc/nginx/nginx.conf
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
daemon off;
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
exit
#######################################################


docker commit -m "mynginx" <nginxv1的docker_id> oldboyedu/mynginx:v2 

docker images # 这个时候v2就是在前台了

docker run -d -p 82:80 oldboyedu/nginxv2:v2 nginx # 前面是镜像,最后的nginx是命令
```


### 6-2. Dockerfile
+ `Dockerfile`首字母大写
+ 里面的命令都大写
+ 状态保持=>第二次执行时会更快
+ 最后是`CMD`,而不是`RUN`,如果最后没有`CMD`的话,创建镜像的时候就会一直卡在那里
#### 6-2-1. Dockerfile书写结构
1. `FROM`:所有Dockfile创建的镜像都必须基于一个已有的镜像(继承)=>妈妈
2. `MAINTAIN`:制作者=>爸爸
3. `RUN`:执行Shell命令(全路径,命令不用全路径)
4. `ADD`:将Dockfile文件实体所在位置的文件传输到镜像里面
5. `WORKDIR`:=cd
6. `VOLUME`:给我一个存放行李的地方,目录挂载
7. `EXPOSE`:镜像要暴露什么端口
8. `CMD`:最终要执行什么命令=>我们在用镜像启动容器的时候,最后都要写命令嘛,这个地方就是那个命令=>我们用该镜像启动容器就不需要重复输入命令了
#### 6-2-2. 利用Dockerfile创建上述nginx版本的容器
Dockfile=>镜像=>容器
1. 创建存放Dockfile的目录A
2. 在该目录A下编辑`Dockfile`文件
3. 需要传送过去的文件也放在该目录A下
4. 利用该目录(不是文件)创建镜像
5. 利用镜像创建并启动容器
6. 验证
```
mkdir /opt/dockerfile/nginx -p
cd /opt/dockerfile/nginx


echo "Hello!!" > index.html

vim Dockerfile
#######################################################
# inherited image(must exists in the local dir or DockerHub)
FROM centos

MAINTAINER orris orris@email.com

RUN rpm -ivh https://mirrors.aliyun.com/epel/epel-release-latest-7.noarch.rpm
RUN yum install -y nginx
ADD index.html /usr/share/nginx/html/index.html
RUN echo "daemon off;">>/etc/nginx/nginx.conf

EXPOSE 80
CMD ["nginx"]
#######################################################


docker build -t oldboyedu/orris_nginx:v3 /opt/dockerfile/nginx

docker run --name nginxv3 -d -p 83:80 oldboyedu/orris_nginx:v3

# 在浏览器里输入`http://192.168.56.10:83`看看结果就行了(该ip地址为我docker的服务器的IP地址)
```

## 7. 仓库
### 7-1. 查看仓库里的镜像文件
```
curl 192.168.56.10:5000/v2/_catalog # 查看仓库里的镜像
```
### 7-2. push的前置条件
#### 7-2-1. 自己配置HTTPS(未成功)
使用openssl自行签发证书 > [参考文档](https://yeasy.gitbooks.io/docker_practice/repository/registry_auth.html)
+ 这里假设我们将要搭建的私有仓库地址为`registry.oldboyedu.com`,下面我们介绍使用 openssl 自行签发`registry.oldboyedu.com`的站点 SSL 证书。
```
# 如果没有nginx包的话,执行这个:rpm -ivh https://mirrors.aliyun.com/epel/epel-release-latest-7.noarch.rpm
yum install -y nginx

# 配置nginx,用户认证https

cd /etc/nginx/conf.d/ # nginx会默认去包含这个目录下的conf文件
vim docker-registry.conf
################################################
upstream docker-registry {
  server 127.0.0.1:5000;
}

server {
  listen 443;
  server_name registry.oldboyedu.com;
  ssl on;
  ssl_certificate /etc/ssl/nginx.crt;
  ssl_certificate_key /etc/ssl/nginx.key;
  proxy_set_header Host $http_host;
  proxy_set_header X-Real-IP $remote_addr;
  client_max_body_size 0;
  chunked_transfer_encoding on;
  location / {
    auth_basic "Docker";
    auth_basic_user_file /etc/nginx/conf.d/docer-registry.htpasswd;
    proxy_pass http://docker-registry;
  }
  location /_ping {
    auth_basic off;
    proxy_pass http://docker-registry;
  }
  location /v1/_ping {
    auth_basic off;
    proxy_pass http://docker-registry;
  }
}
################################################


# 做https的认证(sign和commit的时候按y)

cd /etc/pki/CA
touch ./{serial,index.txt}
echo "00">serial

# 创建一个CA私钥
openssl genrsa -out private/cakey.pem 2048 

# 利用私钥创建 CA 根证书请求文件
openssl req -new -x509 -key private/cakey.pem -days 3650 -out cacert.pem
################################################
CN
Beijing
Beijing
oldboyedu
dockercangku
registry.oldboyedu.com
admin@oldboyedu.com
################################################



cd /etc/ssl
openssl genrsa -out nginx.key 2048
openssl req -new -key nginx.key -out nginx.csr
################################################
CN
Beijing
Beijing
oldboyedu
dockercangku
registry.oldboyedu.com
admin@oldboyedu.com

# challenge password和optional company name跳过
################################################
# 签发证书
openssl ca -in nginx.csr -days 3650 -out nginx.crt

# 让系统接收/承认我们自签发的证书
cat /etc/pki/CA/cacert.pem >>/etc/pki/tls/certs/ca-bundle.crt

# 如果没有htpasswd这个命令的话,需要安装httpd
htpasswd -c /etc/nginx/conf.d/docker-registry.htpasswd oldboy
#########密码设置成123123
123123
123123
#########
systemctl start nginx

netstat -lntup | grep 443 # 如果有的话,就是nginx有了

# 做个绑定
echo "192.168.56.10 registry.oldboyedu.com" >> /etc/hosts

docker login -u oldboy -p 123123 -e admin@oldboyedu.com registry.oldboyedu.com
```
#### 7-2-2. 修改docker配置文件,使允许非HTTPS推送镜像
```
# 如果登录失败的话,就只能修改成ip地址了,否则我们可以直接使用域名push
vim /etc/sysconfig/docker 
####################################
OPTIONS='--selinux-enabled --insecure-registry 192.168.56.10:5000'
####################################
systemctl restart docker 
docker start <docker_id> # 启动registry容器,因为重启docker会暂停所有容器

```

### 7-2. 创建1个仓库
1. 利用官网的registry镜像来做成容器运行仓库
2. Dockfile构建镜像I
3. 给镜像I打上标签 (注意:此时`docker images`里面会有该镜像I_tag,但仓库里没有)
4. 配置HTTPS/修改Docker配置文件使允许非HTTPS的推送
5. 将打上标签的镜像I_tag push到仓库里 (本质上是直接对`docker images`里的完整I_tag做push而已)
6. 其他节点直接pull下来
```
docker pull registry
docker run -d -p 5000:5000 --name registry registry
curl 192.168.56.10:5000/v1/search

# 这里选择将已有的oldboyedu/orris_nginx:v3镜像打上标签后上传到仓库
docker tag oldboyedu/orris_nginx:v3 192.168.56.10:5000/oldboyedu/mynginx:latest
docker push 192.168.56.10:5000/oldboyedu/mynginx:latest
#-----------------------------------------------------------------------------------------
The push refers to a repository [192.168.56.10:5000/oldboyedu/mynginx]
Get https://192.168.56.10:5000/v1/_ping: http: server gave HTTP response to HTTPS client
#-----------------------------------------------------------------------------------------
# 执行push后会报上面的错误,`echo $?`为1
# 这是因为Docker默认不允许非HTTPS方式推送镜像
# 解决方法=>1. 自己配置HTTPS 2. 修改docker配置文件,使允许非HTTPS推送镜像

#######################################################################################
#                                                                                     #
#                  这块区域执行7-2里的push前置条件中的任何一个!!!                           #
#                                                                                     #
#######################################################################################

curl 192.168.56.10:5000/v2/_catalog # 查看仓库里的镜像
docker push 192.168.56.10:5000/oldboyedu/nginx:latest

curl 192.168.56.10:5000/v2/_catalog # 查看仓库里的镜像
#---------------------------------------------------------------------------------------
{"repositories":["oldboyedu/mynginx"]}
#---------------------------------------------------------------------------------------

# 其他节点可以直接pull下来
docker pull 192.168.56.10:5000/oldboyedu/mynginx

```





