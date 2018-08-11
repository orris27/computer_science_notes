## 1. 编译安装
1. 安装
> https://github.com/orris27/orris/blob/master/linux/saltstack/configuration.md
2. 写配置文件
```
sudo mkdir /etc/haproxy
sudo vim haproxy.cfg
##############
# 内容见该文档的下面"配置文件"部分
##############
```
3. 启动服务
```
sudo /usr/local/haproxy/sbin/haproxy -f /etc/haproxy/haproxy.cfg
```

--------------------------------------

## 2. salt安装
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
##############################################################################


##################################################################################################
# 执行salt命令
##################################################################################################
salt '*' state.sls haproxy.install env=prod

```


## 3. 配置文件
1. 外网的haproxy
    + 详细介绍看
    > https://blog.csdn.net/u012758088/article/details/78643704
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

backend backend_www_example_com
    option forwardfor header X-REAL-IP
    option httpchk HEAD / HTTP/1.0
    balance source
    server web-node1 172.19.28.82:8080 check inter 2000 rise 30 fall 15
    server web-node2 172.19.28.84:8080 check inter 2000 rise 30 fall 15
```
