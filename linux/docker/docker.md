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
### 3-2. `nsenter`
1. `-m`:挂载namespace
2. `-n`:进入网络的namespace
3. `-p`:进入pid的namespace
### 3-3. 启动容器
1. 创建并启动容器
+ 镜像的名称
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
#### 5-1-3. 只挂载单个文件(很少用)
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









```

