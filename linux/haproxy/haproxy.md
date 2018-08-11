## 1. 安装
### 1-1. 编译安装
> https://github.com/orris27/orris/blob/master/linux/saltstack/configuration.md


### 1-2. salt安装
> [haproxy的prod目录](https://github.com/orris27/orris/tree/master/linux/haproxy/prod-haproxy)
以下为完整的salt安装过程
1. 安装SaltStack
    1. 安装epel源
    2. 安装salt-master和salt-minion
    3. 自己PC上做主从
2. 准备salt命令
    
    1. salt-master上配置环境
        1. base:`/srv/salt/base`
        2. prod:`/srv/salt/prod`
        3. 创建对应物理目录
    2. 依赖包
        1. 在prod的`pkg`目录下放安装依赖包的状态文件
    3. 编写安装Haproxy的状态文件
        1. 在prod目录下创建haproxy目录,写安装haproxy的状态文件
    4. 将需要传输过去的压缩包等放到`prod/haproxy/files`下
3. 执行salt命令:`salt '*' state.sls haproxy.install env=prod`
 
```
##################################################################################################
# 安装SaltStack.master和minion都是自己10.0.0.7
##################################################################################################
wget -O /etc/yum.repos.d/epel.repo http://mirrors.aliyun.com/repo/epel-7.repo

sudo yum install salt-master salt-minion -y
sudo systemctl start salt-master
sudo systemctl enable salt-master

sudo vim /etc/salt/minion
#####################################################
master: 172.19.28.82
#id:
#####################################################
sudo systemctl start salt-minion
sudo systemctl enable salt-minion

salt-key
salt-key -A

##################################################################################################
# 编写SaltStack
##################################################################################################
vim /etc/salt/master
########################################################
file_roots:
  base:
    - /srv/salt/base
  prod:
    - /srv/salt/prod
########################################################
sudo systemctl restart salt-master
sudo mkdir -p /srv/salt/{base,prod}

mkdir -p /srv/salt/prod/pkg
cat > /srv/salt/prod/pkg/pkg-init.sls <<EOF
pkg-init:
  pkg.installed:
    - names:
      - gcc
      - gcc-c++
      - glibc
      - make
      - autoconf
      - openssl
      - openssl-devel
EOF

cd ~/tools/
wget http://www.haproxy.org/download/1.8/src/haproxy-1.8.12.tar.gz
tar zxf haproxy-1.8.12.tar.gz 
cd haproxy-1.8.12
vim examples/haproxy.init
##############################################################################
# BIN=/usr/sbin/$BASENAME
BIN=/usr/local/haproxy/sbin/$BASENAME
##############################################################################
mkdir -p /srv/salt/prod/haproxy/files
sudo cp ~/tools/haproxy-1.8.12.tar.gz /srv/salt/prod/haproxy/files/
sudo cp ~/tools/haproxy-1.8.12/examples/haproxy.init /srv/salt/prod/haproxy/files/
cd /srv/salt/prod/haproxy
sudo vim install.sls
##############################################################################
include:
  - pkg.pkg-init

haproxy-install:
  file.managed:
    - name: /root/tools/haproxy-1.8.12.tar.gz
    - source: salt://haproxy/files/haproxy-1.8.12.tar.gz
    - user: root
    - group: root
    - mode: 755
  cmd.run:
    - name: cd /root/tools && tar -zxf haproxy-1.8.12.tar.gz && cd haproxy-1.8.12 && sudo make TARGET=linux2628 PREFIX=/usr/local/haproxy-1.8.12 && sudo make install PREFIX=/usr/local/haproxy-1.8.12 && sudo ln -s /usr/local/haproxy-1.8.12/ /usr/local/haproxy
    - unless: test -d /usr/local/haproxy
    - require:
      - pkg: pkg-init
      - file: haproxy-install

haproxy-init:
  file.managed:
    - name: /etc/init.d/haproxy
    - source: salt://haproxy/files/haproxy.init
    - user: root
    - group: root
    - mode: 755
    - require:
      - cmd: haproxy-install
  cmd.run:
    - name: chkconfig --add haproxy
    - unless: chkconfig --list | grep haproxy
    - require:
      - file: haproxy-init


haproxy-config-dir:
  file.directory:
    - name: /etc/haproxy
    - user: root
    - group: root
    - mode: 755

command-link:
  cmd.run:
    - name: ln -s /usr/local/haproxy/sbin/haproxy  /usr/local/bin/
    - require:
      - file: haproxy-config-dir
##############################################################################


##################################################################################################
# 执行salt命令
##################################################################################################
salt '*' state.sls haproxy.install env=prod

```

## 2. 使用
### 2-1. 基本使用
1. 语法检查
```
haproxy -c -f /etc/haproxy/haproxy.cfg
```
2. 启动服务
```
haproxy -f /etc/haproxy/haproxy.cfg
```

3. 停止服务
```
kill `cat /var/run/haproxy.pid`
```
### 2-2. 根据URL匹配不同的后台服务器
```
frontend xxxx
    acl is_other_example_com hdr_end(host) other.example.com
    use_backend backend_another_example_com if is_other_example_com
```

## 3. 配置文件
1. 外网的haproxy
    + 详细介绍看
    > https://blog.csdn.net/u012758088/article/details/78643704
2. 说明
    1. `global`
        1. `log 127.0.0.1 local3 info`:设置日志
        2. `daemon`:说明以守护进程的形式启动
    2. `defaults`:默认参数可以被frontend和backend继承.比如如果defaults里面设置了`mode http`,那么frontend里面就不用设置`mode http`
        1. `mode`:可以是tcp也可以是http
        2. `option httplog`:记录http的日志
        3. `option dontlognull`:不记录健康检查的日志
    3. `frontend`:
        1. `stats uri /haproxy?stats`:dashboard使用的uri
        2. `acl`:指定ACL,可以匹配URL
        3. `use_backend`:根据ACL匹配来选择后台服务器
    4. `backend`:
        1. `option`
            1. `httpchk GET /index.html`:指定健康检查的方式,这里指测试index.html.不写的话是检查端口
            2. `forward for header X-REAL-IP`:保持用户的IP
            
        2. `balance`
            1. `source`:ip哈希
            2. `roundrobin`:轮询
        3. `server`:
            1. `check`:健康检查
            2. `inter`:健康检查间隔
            3. `rise 3`:测试次数.测试成功3次才认为是健康的
            4. `fail 3`:测试失败3次就认为是失败的
            5. `weight 1`:权重是1
        
```
global
    log 127.0.0.1 local3 info
    uid 99
    gid 99
    daemon
    nbproc 1
    maxconn 100000
    pidfile /var/run/haproxy.pid
    chroot /usr/local/haproxy
defaults
    mode http
    option http-keep-alive
    option httplog
    option dontlognull
    timeout connect 5000ms
    timeout client 50000ms
    timeout server 50000ms

listen stats
    bind 0.0.0.0:8888
    mode http
    stats uri /haproxy-status
    stats auth haproxy:saltstack
    stats hide-version
    stats enable

frontend frontend_www_example_com
    bind 172.19.28.82:80
    mode http
    log global
    option httplog
    default_backend backend_www_example_com
    acl is_other_example_com hdr_end(host) other.example.com
    use_backend backend_another_example_com if is_other_example_com # 使用这个主机,如果匹配到指定ACL

backend backend_www_example_com
    option forwardfor header X-REAL-IP
    option httpchk HEAD / HTTP/1.0
    balance source
    server web-node1 172.19.28.82:8080 check inter 2000 rise 30 fall 15
    server web-node2 172.19.28.84:8080 check inter 2000 rise 30 fall 15

backend backend_another_example_com
    option forwardfor header X-REAL-IP
    option httpchk HEAD / HTTP/1.0
    balance source
    server web-node1 172.19.28.82:8080 check inter 2000 rise 30 fall 15
    server web-node2 172.19.28.84:8080 check inter 2000 rise 30 fall 15


```


## 4. 配置
```

useradd -M -s /sbin/nologin haproxy
sudo mkdir /etc/haproxy
sudo vim /etc/haproxy/haproxy.cfg
############################################################################################
# 可以参考上面的配置文件
global
    log 127.0.0.1 local3 info
    user haproxy
    group haproxy
    daemon
    nbproc 1
    maxconn 100000
    pidfile /var/run/haproxy.pid
    chroot /usr/local/haproxy
    
defaults
    log global
    mode http
    option httplog
    option dontlognull
    timeout connect 5000ms
    timeout client 50000ms
    timeout server 50000ms

frontend http_front
    bind *:80
    stats uri /haproxy?stats
    default_backend http_backend

backend http_backend
    option httpchk GET /index.html
    balance roundrobin
    server web-node1 10.0.0.8:80 check inter 2000 rise 30 fall 15
    server web-node2 10.0.0.9:80 check inter 2000 rise 30 fall 15
############################################################################################

vim /etc/rsyslog.conf
############################################################################################
$ModLoad imudp
$UDPServerRun 514


local3.*                                                /var/log/haproxy.log
############################################################################################
systemctl restart rsyslog

haproxy -c -f /etc/haproxy/haproxy.cfg
haproxy -f /etc/haproxy/haproxy.cfg # 启动haproxy

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 状态可以通过访问http:10.0.0.7/haproxy?stats
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

curl 10.0.0.7
```
