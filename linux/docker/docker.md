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
3. `-i`:保持打开的状态,`-t -i`一起执行
### 3-2. `nsenter`
1. `-m`:挂载namespace
2. `-n`:进入网络的namespace
3. `-p`:进入pid的namespace
### 3-2. 启动容器
1. 创建并启动容器
+ 镜像的名称
+ 执行的命令
```
docker run centos /bin/echo "hello world"
docker run --rm centos /bin/echo "hello world" # 执行结束后会自动删除容器=>防止我们启动太多容器占用资源
```

2. 使用存在的容器启动
```
docker start <docker_id>
```

3. 查看帮助
```
docker run --help
```

4. 容器的名称
+ 容器有名称+ID=>容器的主机名=容器的ID
+ 容器的名称如果不指定会自动生成
```
docker run --name mydocker -t -i centos /bin/bash
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
  


