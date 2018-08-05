## 1. 安装
+ 整个安装参考[他人文档](http://blog.51cto.com/andyliu/2154044)+[官方文档](https://docs.openstack.org/keystone/queens/install/keystone-install-rdo.html)
+ 第一次keystone的失败原因是在`/etc/keystone/keystone.conf`中让token和revoke使用了`memcache`,结果就导致了`Failed to discover available identity versions when contacting http://controller:35357/v3. Attempting to parse version from URL.Internal Server Error (HTTP 500)`错误
+ 使用`/etc/keystone/keystone.conf`的话,要重启httpd
1. 环境准备
    1. CentOS-7.5 系统2台,每台2G内存
    + controller 192.168.56.11 网卡NAT eth0 控制节点
    + computer 192.168.56.12 网卡NAT eth0 计算节点
        1. NAT实现外网+与物理机不同网段的实现
        + vment8       != 菜单栏的网卡   = 虚拟机的网卡  != 虚拟机的IP地址
        + 192.168.56.1 != 192.168.56.2 =192.168.56.2 != 192.168.56.11        
        > https://github.com/orris27/orris/blob/master/linux/vmware/installation.md
        2. 改变网卡名称
        > https://github.com/orris27/orris/blob/master/network/network.md
        
    
    2. 域名解析
    + OpenStack主机名很重要
    ```
    cat >> /etc/hosts <<EOF
    192.168.56.11 controller
    192.168.56.12 compute
    EOF
    ```
    3. 硬盘50G
    
    4. Tightvnc view VNC客户端
    
    5. 安装包:老师发给大家
    
    6. 理解SOA,RestAPI,消息队列,对象存储
    
    7. 使用Queens版OpenStack


2. 同步时间
```
##################################
# 控制节点
##################################
yum install chrony -y
vim /etc/chrony.conf
####################
allow  192.168/16 # 取消注释就行了
####################
systemctl enable chronyd.service
systemctl start chronyd.service

timedatectl set-
timezone Asia/Shanghai
date # 和当前的时间一样
```

3. 安装依赖包
```
yum install centos-release-openstack-queens -y
yum install python-openstackclient -y
yum install openstack-selinux -y
yum install openstack-utils -y
yum install python2-PyMySQL -y

```

4. 安装数据库
    1. yum安装数据库
    2. 修改数据库的配置文件
    3. 创建db和新用户
    + yum源可以考虑使用阿里的源,执行`yum install -y wget;mv /etc/yum.repos.d/CentOS-Base.repo /etc/yum.repos.d/CentOS-Base.repo.bak;wget -O /etc/yum.repos.d/CentOS-Base.repo http://mirrors.aliyun.com/repo/Centos-7.repo`
    ```
    yum install -y mariadb mariadb-server MySQL-python
    cp /usr/share/mysql/my-medium.cnf /etc/my.cnf
    vim /etc/my.cnf
    ######################################## 下面的内容添加到/etc/my.cnf的[mysqld]下面
    default-storage-engine = innodb
    innodb_file_per_table
    collation-server = utf8_general_ci
    init-connect = 'SET NAMES utf8'
    character-set-server = utf8
    ########################################
    systemctl enable mariadb.servicem
    systemctl start mariadb.service # 启动的服务还是叫mysqld

    mysql_secure_installation
    # 设置当前密码:Y
    # 输入密码
    # 移除匿名用户:Y
    # 关闭root远程登录:Y
    # 删除默认的test数据:Y
    # 刷新权限:Y
    mysql -u root -p 
    #######################wendang########################
    create database keystone;ok
    grant all privileges on keystone.* to 'keystone'@'localhost' identified by 'keystone';
    grant all privileges on keystone.* to 'keystone'@'%' identified by 'keystone';

    create database glance;
    grant all privileges on glance.* to 'glance'@'localhost' identified by 'glance';
    grant all privileges on glance.* to 'glance'@'%' identified by 'glance';

    create database nova;
    grant all privileges on nova.* to 'nova'@'localhost' identified by 'nova';
    grant all privileges on nova.* to 'nova'@'%' identified by 'nova';

    create database neutron;
    grant all privileges on neutron.* to 'neutron'@'localhost' identified by 'neutron';
    grant all privileges on neutron.* to 'neutron'@'%' identified by 'neutron';

    create database cinder;
    grant all privileges on cinder.* to 'cinder'@'localhost' identified by 'cinder';
    grant all privileges on cinder.* to 'cinder'@'%' identified by 'cinder';

    show databases;

    ###############################################
    ```



4. 安装消息队列(交通枢纽)
    1. 安装Rabbitmq
    + Rabbitmq可以做集群
    ```
    yum install -y rabbitmq-server
    ```
    2. 启动Rabbitmq服务
    + rabbitmq-server启动服务后端口是5672
    ```
    systemctl enable rabbitmq-server.service
    systemctl start rabbitmq-server.service
    systemctl status rabbitmq-server.service
    ```
    3. 添加消息队列的认证
    + 消息队列肯定要有认证系统
    ```
    netstat -lntup | grep 5672

    rabbitmqctl add_user openstack openstack
    rabbitmqctl set_permissions openstack ".*" ".*" ".*"
    rabbitmq-plugins list
    rabbitmq-plugins enable rabbitmq_management
    systemctl restart rabbitmq-server.service
    ```
    4. 检查网络状态
    ```
    netstat -lntup | grep 15672
    ```
    5. 关闭防火墙后,浏览器访问`192.168.56.11:15672`(Web管理页面)
    + 默认的用户密码都是guest
    + Admin这里赋值admin的tags(administrator),然后点击openstack,然后在Update this user的地方,复制刚才的tags进这个的Tags并修改密码,然后update
    
    6. 浏览器访问`192.168.56.11:15672`,然后可以输入刚才的用户名和密码(如openstack:openstack)
    
    7. (可选)监听的话,使用`HTTP api`(在网页的最下面,很小的字)
    
    
    
5. 安装memcache
```ok
yum install memcached python-memcached -y
vim /etc/sysconfig/memcached
##################################################
OPTIONS="-l 127.0.0.1,::1,controller"
##################################################
systemctl enable memcached.service
systemctl start memcached.service
systemctl status memcached.service
```
    
6. 安装etcd
+ 他人文档里写的是`ETCD_NAME="controller"`,但这样会报错
+ 日志文件:`/var/log/messages`
```
yum install etcd -y
vim /etc/etcd/etcd.conf
########################################################################
ETCD_DATA_DIR="/var/lib/etcd/default.etcd"
ETCD_LISTEN_PEER_URLS="http://192.168.56.11:2380"
ETCD_LISTEN_CLIENT_URLS="http://192.168.56.11:2379"
ETCD_NAME="default"
ETCD_INITIAL_ADVERTISE_PEER_URLS="http://192.168.56.11:2380"
ETCD_ADVERTISE_CLIENT_URLS="http://192.168.56.11:2379"
ETCD_INITIAL_CLUSTER="default=http://192.168.56.11:2380"
ETCD_INITIAL_CLUSTER_TOKEN="etcd-cluster-01"
ETCD_INITIAL_CLUSTER_STATE="new"
########################################################################
grep -v "^#" /etc/etcd/etcd.conf
systemctl enable etcd
systemctl start etcd
systemctl status etcd
```



7. 部署keystone(验证服务)>[官方文档](https://docs.openstack.org/keystone/queens/install/keystone-install-rdo.html)
    1. 安装OpenStack的queens版本的rpm包
    + `No+package+openstack-keystone+available.`的错误是因为没有安装足够的源
    + 以前的如juno等都不可用了,可以在下面链接里查看可用的版本
    > https://repos.fedorapeople.org/repos/openstack
    ```
    yum install openstack-keystone httpd mod_wsgi -y --enablerepo=base
    ```
    2. 配置keystone的配置文件
    + 前面不能有空格
    + 配置数据库=>保存验证信息
        - 用户名:密码@主机/数据库名
    + 修改`keystone.conf`后要重启httpd服务
    + 不能修改token和revoke的driver为memcache,否则会500错误
    ```
    vim /etc/keystone/keystone.conf
    #######################################
    [database]
    connection = connection = mysql+pymysql://keystone:keystone@controller/keystone
    [token]
    provider = fernet
    #######################################
    ```

    3. 导入数据库
    + 切换到keystone用户是因为让下面的日志文件的属主为keystone,这样就可以读写日志文件
    ```
    su -s /bin/sh -c "keystone-manage db_sync" keystone
    # tail /var/log/keystone/keystone.log

    # Initialize Fernet key repositories
    keystone-manage fernet_setup --keystone-user keystone --keystone-group keystone
    keystone-manage credential_setup --keystone-user keystone --keystone-group keystone
    
    
    mysql -h 192.168.56.11 -u keystone -pkeystone
    ######################
    use keysmtone;
    show tables; # 如果出现表就说明导入数据库成功了
    ######################
    ```
    
    4. Bootstrap the Identity service
    + 密码变成了`ADMIN_PASS`...
    ```
    keystone-manage bootstrap --bootstrap-password ADMIN_PASS   --bootstrap-admin-url http://controller:5000/v3/   --bootstrap-internal-url http://controller:5000/v3/   --bootstrap-public-url http://controller:5000/v3/   --bootstrap-region-id RegionOne
    ```
    
    5. 修改httpd配置文件
        1. 让httpd服务指向controller节点
        ```
        vim /etc/httpd/conf/httpd.conf
        ###################################
        ServerName controller
        ###################################
        ```
        2. 引入OpenStack的web配置文件
        ```
        ln -s /usr/share/keystone/wsgi-keystone.conf /etc/httpd/conf.d/
        ```

    6. 启动httpd服务
    ```
    systemctl enable httpd.service
    systemctl start httpd.service
    systemctl status httpd.service
    ```
    
    7. 输入登录用的账户(基于环境变量)
    ```
    export OS_USERNAME=admin
    export OS_PASSWORD=ADMIN_PASS
    export OS_PROJECT_NAME=admin
    export OS_USER_DOMAIN_NAME=Default
    export OS_PROJECT_DOMAIN_NAME=Default
    export OS_AUTH_URL=http://controller:35357/v3
    export OS_IDENTITY_API_VERSION=3
    ```
    
    8. 验证
    ```
    openstack domain create --description "An Example Domain" example # 如果输出id等表格,就说明ok
    ```
    
    
    9. 创建管理员和普通用户的project,user,role
    ```
    openstack project create --domain default --description "Admin Project" admin
    openstack user create --domain default --password-prompt admin # 这里密码简单地设置为admin
    openstack role create admin
    openstack role add --project admin --user admin admin

    openstack project create --domain default --description "Demo Project" demo
    openstack user create --domain default --password=demo demo
    openstack role create user
    openstack role add --project demo --user demo user # demo用户加入到demo项目中,赋予user角色
    
    openstack project create --domain default --description "Service Project" service
    
    openstack user list
    openstack role list
    openstack project list
    
    ```
    
    10. 加入keystone服务到keystone里(他人文档和官方文档里没有这个步骤,所以我先跳过了)
    + 公共:互联网上,可以对外(5000+v2)
    + 内部的:内部用(5000+v2)
    + 管理:内部用(35357+v3),但好像还是v3
    ```
    openstack service create --name keystone --description "Openstack Identity" identity # 最后一个为类型,不能写错
    openstack endpoint create --region RegionOne identity public http://192.168.56.11:5000/v2.0
    openstack endpoint create --region RegionOne identity internal http://192.168.56.11:5000/v2.0
    openstack endpoint create --region RegionOne identity admin http://192.168.56.11:35357/v2.0
    
    openstack endpoint list
    # openstack endpoint delete <id> # 可以删除
    ```
    
    11. 连接上OpenStack拿token完
        1. 去除原来的环境变量(一定要去掉) 
        + 不去除的话会冲突
        + `#-------`范围内的是老师上课讲的,其余是官方文档/他人文档的,我采用的是官方文档/他人文档
        ```
        unset OS_AUTH_URL OS_PASSWORD
        ```
        2. 连接OpenStack
        + 只有这里获取到tokens才能说keystone创建成功
        + controller写成`192.168.56.11`也可以
        + Default小写default也可以
        ```
        #################################################################
        # admin
        #################################################################
        openstack --os-auth-url http://controller:35357/v3 \
        --os-project-domain-name Default \
        --os-user-domain-name Default \
        --os-project-name admin \
        --os-username admin \
        token issue # 我的密码是ADMIN_PASS(在bootstrap那里设置的...)
        
        #################################################################
        # demo
        #################################################################
        openstack --os-auth-url http://controller:35357/v3 \
        --os-project-domain-name Default \
        --os-user-domain-name Default \
        --os-project-name demo \
        --os-username demo \
        token issue # 密码是demo
        ```
        
        3. 配置keystone环境变量,方便执行命令
        ```
        
        cd ~
        vim admin-openrc.sh
        ######################################################
        export OS_PROJECT_DOMAIN_NAME=Default
        export OS_USER_DOMAIN_NAME=Default
        export OS_PROJECT_NAME=admin
        export OS_USERNAME=admin
        export OS_PASSWORD=ADMIN_PASS
        export OS_AUTH_URL=http://controller:5000/v3
        export OS_IDENTITY_API_VERSION=3
        export OS_IMAGE_API_VERSION=2
        ######################################################
        
        vim demo-openrc.sh
        ######################################################
        export OS_PROJECT_DOMAIN_NAME=Default
        export OS_USER_DOMAIN_NAME=Default
        export OS_PROJECT_NAME=demo
        export OS_USERNAME=demo
        export OS_PASSWORD=demo
        export OS_AUTH_URL=http://controller:5000/v3
        export OS_IDENTITY_API_VERSION=3
        export OS_IMAGE_API_VERSION=2
        ######################################################
        
        source admin-openrc.sh
        openstack token issue # 这样就可以执行了
        ```
    
    
8. glance > [官方文档](https://docs.openstack.org/glance/queens/install/install-rdo.html)
    1. 安装glance的yum包
    ```
    yum install openstack-glance -y
    #----------------------------------------------------------------------
    #yum install -y openstack-glance python-glance python-glanceclient
    #----------------------------------------------------------------------
    ```
    
        
    2.在keystone上创建服务
    ```
    source ~/admin-openrc.sh
    openstack user create --domain default --password=glance glance # 如果报错说明现在的环境变量有问题,应该source admin-openrc.sh
    openstack role add --project service --user glance admin 

    openstack service create --name glance --description "OpenStack Image" image
    
    openstack endpoint create --region RegionOne image public http://controller:9292
    openstack endpoint create --region RegionOne image internal http://controller:9292
    openstack endpoint create --region RegionOne image admin http://controller:9292
    ```
    
    3. 配置数据库
    ```
    vim /etc/glance/glance-api.conf
    ###########################################
    [database]
    connection = mysql+pymysql://glance:glance@controller/glance
    ###########################################
    
    vim /etc/glance/glance-registry.conf
    ###########################################
    [database]
    connection = mysql+pymysql://glance:glance@controller/glance
    ###########################################
    
    su -s /bin/sh -c "glance-manage db_sync" glance # No handlers提示可以忽略
    
    mysql -uglance -pglance
    ###########################################
    use glance;
    show tables;
    exit;
    ###########################################
    ```
    
    4. 配置keystone
    ```
    vim /etc/glance/glance-api.conf
    ###########################################
    #其他都是注释的
    [keystone_authtoken]
    auth_uri = http://controller:5000
    auth_url = http://controller:5000
    memcached_servers = controller:11211
    auth_type = password
    project_domain_name = Default
    user_domain_name = Default
    project_name = service
    username = glance
    password = glance
    
    [paste_deploy]
    flavor = keystone
    
    [glance_store]
    stores = file,http
    default_store = file
    filesystem_store_datadir = /var/lib/glance/images/
    #----------------------------------------------------------
    #其他都是注释的
    #auth_uri = http://192.168.56.11:5000
    #auth_url = http://192.168.56.11:35357
    #auth_plugin = password
    #project_domain_id = default
    #user_domain_id = default
    #project_name = service
    #username = glance
    #password = glance
    
    ##...egrep -v '^$|^#' /etc/glance/glance-api.conf 
    #notification_driver = noop # glance不需要用到消息队列
    
    ##...
    #verbose=True
    #----------------------------------------------------------
    
    ###########################################
    
    vim /etc/glance/glance-registry.conf
    ###########################################
    [keystone_authtoken]
    auth_uri = http://controller:5000
    auth_url = http://controller:5000
    memcached_servers = controller:11211
    auth_type = password
    project_domain_name = Default
    user_domain_name = Default
    project_name = service
    username = glance
    password = glance

    [paste_deploy]
    flavor = keystone
    
    #------------------------------------------
    #[keystone_authtoken]
    ## 其他都是注释的
    #auth_uri = http://192.168.56.11:5000
    #auth_url = http://192.168.56.11:35357
    #auth_plugin = password
    #project_domain_id = default
    #user_domain_id = default
    #project_name = service
    #username = glance
    #password = glance

    #------------------------------------------
    ###########################################
    
    
    
    
    
    
    
    
    # 验证
    egrep -v '^$|^#' /etc/glance/glance-api.conf 
    ###########################################
    [DEFAULT]
    [cors]
    [database]
    connection = mysql+pymysql://glance:glance@controller/glance
    [glance_store]
    stores = file,http
    default_store = file
    filesystem_store_datadir = /var/lib/glance/images
    [image_format]
    echo "export OS_IMAGE_API_VERSION=2" >> demo-openrc.sh
    [keystone_authtoken]
    auth_uri = http://controller:5000
    auth_url = http://controller:5000
    memcached_servers = controller:11211
    auth_type = password
    project_domain_name = Default
    user_domain_name = Default
    project_name = service
    username = glance
    password = glance
    [matchmaker_redis]
    [oslo_concurrency]
    [oslo_messaging_amqp]
    [oslo_messaging_kafka]
    [oslo_messaging_notifications]
    [oslo_messaging_rabbit]
    [oslo_messaging_zmq]
    [oslo_middleware]
    [oslo_policy]
    [paste_deploy]
    flavor = keystone
    [profiler]
    [store_type_location_strategy]
    [task]
    [taskflow_executor]
    ###########################################
    ```
    
    
    5. 启动glance
    ```
    systemctl enable openstack-glance-api
    systemctl enable openstack-glance-registry
    systemctl start openstack-glance-api
    systemctl start openstack-glance-registry
    systemctl status openstack-glance-api
    systemctl status openstack-glance-registry
    
    netstat -lntup | grep 9191 # registry
    netstat -lntup | grep 9292 # api
    ```

    
    6. 加下环境变量
    ```
    cd ~
    #echo "export OS_IMAGE_API_VERSION=2" >> admin-openrc.sh # 之前写openrc的时候就已经写入了
    #echo "export OS_IMAGE_API_VERSION=2" >> demo-openrc.sh
    cat admin-openrc.sh 
    cat demo-openrc.sh
    #glance image-list # 如果出现表格就说明成功了.空是因为还没有上传镜像
    openstack image list
    ```
    
    
    7. 上传一个镜像测试
    ```
    wget http://download.cirros-cloud.net/0.4.0/cirros-0.4.0-x86_64-disk.img
    
    openstack image create "cirros" \
    --file cirros-0.4.0-x86_64-disk.img \
    --disk-format qcow2 \
    --container-format bare \
    --public
    
    
    #glance image-list # 老版本使用,新版本使用openstack image list
    openstack image list # 如果出现一行就ok了
    ll /var/lib/glance/images
    ```
    

9. Nova
    1. 安装依赖包
    ```
    #-------------------------------------------------------
    yum install -y openstack-nova-api openstack-nova-cert \
    openstack-nova-conductor openstack-nova-console \
    openstack-nova-novncproxy openstack-nova-scheduler \
    python-novaclient
    #-------------------------------------------------------

    yum install openstack-nova-api openstack-nova-conductor \
    openstack-nova-console openstack-nova-novncproxy \
    openstack-nova-scheduler openstack-nova-placement-api
    ```
    2. 准备数据库
    ```
    mysql -uroot -p
    ###################################################################
    CREATE DATABASE nova_api;
    CREATE DATABASE nova;
    CREATE DATABASE nova_cell0;
    
    GRANT ALL PRIVILEGES ON nova_api.* TO 'nova'@'localhost' \
      IDENTIFIED BY 'nova';
    GRANT ALL PRIVILEGES ON nova_api.* TO 'nova'@'%' \
      IDENTIFIED BY 'nova';

    GRANT ALL PRIVILEGES ON nova.* TO 'nova'@'localhost' \
      IDENTIFIED BY 'nova';
    GRANT ALL PRIVILEGES ON nova.* TO 'nova'@'%' \
      IDENTIFIED BY 'nova';

    GRANT ALL PRIVILEGES ON nova_cell0.* TO 'nova'@'localhost' \
      IDENTIFIED BY 'nova';
    GRANT ALL PRIVILEGES ON nova_cell0.* TO 'nova'@'%' \
      IDENTIFIED BY 'nova';
    
    
    exit;
    ###################################################################
    
    ```
    3. 在keystone上创建用户nova和placement
    ```
    source ~/admin-openrc.sh
    openstack user create --domain default --password=nova nova # 如果报错说明现在的环境变量有问题,应该source admin-openrc.sh
    openstack role add --project service --user nova admin
    echo $?
    
    
    openstack service create --name nova --description "OpenStack Compute" compute
    
    #-------------------------------------------------------------------------------------------------------------
    #openstack endpoint create --region RegionOne compute public http://192.168.56.11:8774/v2/%\(tenant_id\)s
    #openstack endpoint create --region RegionOne compute internal http://192.168.56.11:8774/v2/%\(tenant_id\)s
    #openstack endpoint create --region RegionOne compute admin http://192.168.56.11:8774/v2/%\(tenant_id\)s
    #-------------------------------------------------------------------------------------------------------------

    openstack endpoint create --region RegionOne compute public http://controller:8774/v2.1
    openstack endpoint create --region RegionOne compute internal http://controller:8774/v2.1
    openstack endpoint create --region RegionOne compute admin http://controller:8774/v2.1
    
    
    
    openstack user create --domain default --password=placement placement
    openstack role add --project service --user placement admin
    openstack service create --name placement --description "Placement API" placement

    openstack endpoint create --region RegionOne placement public http://controller:8778
    openstack endpoint create --region RegionOne placement internal http://controller:8778
    openstack endpoint create --region RegionOne placement admin http://controller:8778
    ```
    
    
    4. 配置文件
        1. 数据库
        ```
        vim /etc/nova/nova.conf
        ###########################################
        [api_database]
        connection = mysql+pymysql://nova:nova@controller/nova_api
        
        [database]
        connection = mysql+pymysql://nova:nova@controller/nova
        ###########################################
        
        
        

        ```
        2. keystone
        ```
        vim /etc/nova/nova.conf
        ###########################################
        [api]
        auth_strategy = keystone

        [keystone_authtoken]
        auth_url = http://controller:5000/v3
        memcached_servers = controller:11211
        auth_type = password
        project_domain_name = default
        user_domain_name = default
        project_name = service
        username = nova
        password = nova
        
        
        #---------------------------------------------------
        #[keystone_authtoken]
        #auth_uri = http://192.168.56.11:5000
        #auth_url = http://192.168.56.11:35357
        #auth_plugin = password
        #project_domain_id = default
        #user_domain_id = default
        #project_name = service
        #username = nova
        #password = nova
        #
        #[DEFAULT]
        #auth_strategy=keystnoe # 一定要在default下面
        #---------------------------------------------------
        ###########################################
        ```
        3. rabbitmq
        ```
        vim /etc/nova/nova.conf
        ###########################################
        [DEFAULT]
        transport_url = rabbit://openstack:openstack@controller # 根据rabbitmq密码而定
        
        
        #---------------------------------------------------
        rpc_backend=rabbit
        
        [oslo_messaging_rabbit]
        rabbit_host=192.168.56.11
        rabbit_port=5672
        rabbit_userid=openstack
        rabbit_password=openstack
        ###########################################
        ```
        4. 网络
        ```
        vim /etc/nova/nova.conf
        ###########################################
        [DEFAULT]
        use_neutron = True
        firewall_driver = nova.virt.firewall.NoopFirewallDriver # 用Neutron来实现防火墙,而非Nova
        
        #----------------------------------------------------------------------------------------
        #network_api_class=nova.network.neutronv2.api.API # 是python的site-packages的nova目录结构
        
        #security_group_api=neutron
        
        ## 注意要取消注释+添加Neutron
        #linuxnet_interface_driver=nova.network.linux_net.NeutronLinuxBridgeInterfaceDriver
        #----------------------------------------------------------------------------------------
        
        ###########################################
        
        ```
        5. vnc
        ```
        [DEFAULT]
        enabled_apis=osapi_compute,metdata # 删掉ec2,因为我们没有
        my_ip=192.168.56.11
        
        [vnc]
        enabled = true
        # ...
        server_listen = $my_ip
        server_proxyclient_address = $my_ip
        
        
        [glance]
        # ...
        api_servers = http://controller:9292
        
        [oslo_concurrency]
        # ...
        lock_path = /var/lib/nova/tmp
        
        [placement]
        # ...
        os_region_name = RegionOne
        project_domain_name = Default
        project_name = service
        auth_type = password
        user_domain_name = Default
        auth_url = http://controller:5000/v3
        username = placement
        password = placement
        #------------------------------------------
        
        #vncserver_listen=$my_ip
        
        #vncserver_proxyclient_address=$my_ip
        
        #lock_path=/var/lib/nova/tmp

        
        
        #[glance]
        #host=$my_ip
        
        #--------------------------------------------
        ```
        
    5. 由于bug,所以要手动追加下面内容
    ```
    sudo vim /etc/httpd/conf.d/00-nova-placement-api.conf
    ###########################################################
    <Directory /usr/bin>
       <IfVersion >= 2.4>
          Require all granted
       </IfVersion>
       <IfVersion < 2.4>
          Order allow,deny
          Allow from all
       </IfVersion>
    </Directory>
    ###########################################################
    systemctl restart httpd
    ```
    6. 启动服务
        1. 数据库导入
        + `/usr/lib/python2.7/site-packages/oslo_db/sqlalchemy/enginefacade.py:332: NotSupportedWarning: Configuration option(s) ['use_tpool'] not supported.exception.NotSupportedWarning`,如果报这个警告,可以无视
        ```
        su -s /bin/sh -c "nova-manage api_db sync" nova
        su -s /bin/sh -c "nova-manage cell_v2 map_cell0" nova
        su -s /bin/sh -c "nova-manage cell_v2 create_cell --name=cell1 --verbose" nova 
        su -s /bin/sh -c "nova-manage db sync" nova
        
        
        
        nova-manage cell_v2 list_cells # 确认cell0和cell1注册成功
        mysql -unova -pnova
        ###########################################
        use nova;
        show tables;
        ###########################################
        ```
        2. 启动nova服务
        ```
        systemctl enable openstack-nova-api.service \
          openstack-nova-consoleauth.service openstack-nova-scheduler.service \
          openstack-nova-conductor.service openstack-nova-novncproxy.service
        systemctl start openstack-nova-api.service \
          openstack-nova-consoleauth.service openstack-nova-scheduler.service \
          openstack-nova-conductor.service openstack-nova-novncproxy.service
        systemctl status openstack-nova-api.service \
          openstack-nova-consoleauth.service openstack-nova-scheduler.service \
          openstack-nova-conductor.service openstack-nova-novncproxy.service
          
          
        #---------------------------------------------------
        #systemctl enable openstack-nova-api.service \
            openstack-nova-consoleauth.service \
            openstack-nova-cert.service \
            openstack-nova-scheduler.service \
            openstack-nova-conductor.service \
            openstack-nova-novncproxy.service
        #systemctl start openstack-nova-api.service \
            openstack-nova-consoleauth.service \
            openstack-nova-cert.service \
            openstack-nova-scheduler.service \
            openstack-nova-conductor.service \
            openstack-nova-novncproxy.service
        #systemctl status openstack-nova-api.service \
            openstack-nova-consoleauth.service \
            openstack-nova-cert.service \
            openstack-nova-scheduler.service \
            openstack-nova-conductor.service \
            openstack-nova-novncproxy.service
        #---------------------------------------------------
        ```
    7. 验证
    ```
    openstack host list # 列出4个就算正常了
    ```
10. Compute节点 > [官方文档](https://docs.openstack.org/nova/queens/install/compute-install-rdo.html)
+ 不特别说明,其他章节都是针对控制节点,而该章节针对Compute节点

    1. 安装依赖包
    ```
    yum install centos-release-openstack-queens -y
    yum install openstack-nova-compute -y
    ```
    2. 在控制节点将nova的配置文件传输过去
    ```
    
    
    vim /etc/nova/nova.conf
    ##################################################################
    [DEFAULT]
    enabled_apis = osapi_compute,metadata
    use_neutron = True
    firewall_driver = nova.virt.firewall.NoopFirewallDriver
    my_ip = 192.168.56.12
    transport_url = rabbit://openstack:openstack@192.168.56.11
    
    [api]
    # ...
    auth_strategy = keystone

    [keystone_authtoken]
    # ...
    auth_url = http://192.168.56.11:5000/v3
    memcached_servers = 192.168.56.11:11211
    auth_type = password
    project_domain_name = default
    user_domain_name = default
    project_name = service
    username = nova
    password = nova
    
    [vnc]
    # ...
    enabled = true
    server_listen = 0.0.0.0
    server_proxyclient_address = 192.168.56.12
    novncproxy_base_url = http://192.168.56.11:6080/vnc_auto.html
    
    [glance]
    # ...
    api_servers = http://192.168.56.11:9292
    
    [oslo_concurrency]
    # ...
    lock_path = /var/lib/nova/tmp
    
    [placement]
    # ...
    os_region_name = RegionOne
    project_domain_name = Default
    project_name = service
    auth_type = password
    user_domain_name = Default
    auth_url = http://192.168.56.11:5000/v3
    username = placement
    password = placement
    
    [libvirt]
    virt_type = kvm
    ##################################################################
    
    
    
    #------------------------------------------------------------------------
    ######################################
    # Controller
    ######################################
    scp /etc/nova/nova.conf 192.168.56.12:/etc/nova/
    
    
    
    
    ######################################
    # Compute
    ######################################
    
    vim /etc/nova/nova.conf
    #########################################
    my_ip=192.168.56.12
    
    novncproxy_base_url=http://192.168.56.11:6080/vnc_auto,html 
    vncserver_listen=0.0.0.0
    vncserver_proxyclient_address=192.168.56.12
    vnc_enabled=true
    vnc_keymap=en-us  
    
    virt_type=qemu # grep -E '(vmx|svm)' /proc/cpuinfo,没有的话,只能用qemu
    
    [glance]
    host=192.168.56.11
    
    #------------------------------------------------------------------------
    
    #########################################
    
    ```
    3. 添加计算机节点
    ```
    ############################################################################################################
    # Controller
    ############################################################################################################
    . admin-openrc
    
    openstack compute service list --service nova-compute
    su -s /bin/sh -c "nova-manage cell_v2 discover_hosts --verbose" nova
    
    nova-manage cell_v2 discover_hosts # run on the controller node to register those new compute nodes. 
    ```
    4. 和Controller节点同步时间
    ```
    yum install -y chrony
    vim /etc/chrony.conf
    #############################################
    # 删除所有
    server 192.168.56.11 iburst
    #############################################
    timedatectl set-timezone Asia/Shanghai
    
    systemctl enable chronyd.service
    systemctl start chronyd.service
    
    chronyc sources # 验证
    ```
    5. 启动计算节点
    + 如果启动不了的话,尝试下面方法
        1. 看`/var/log/messages`:如果不能打开`/etc/nova/nova.conf`=>没有权限=>查看属主,发现root:root,改成root:nova
        2. 看`/var/log/nova/nova-compute.log`:如果说`AMQP server on controller:5672 is unreachable`,说明controller开了防火墙=>关闭防火墙
        3. 看`/var/log/nova/nova-compute.log`:如果说`MessageDeliveryFailure: Unable to connect to AMQP server on 192.168.56.11:5672 after None tries: (0, 0): (403) ACCESS_REFUSED - Login was refused using authentication mechanism AMQPLAIN. For details see the broker logfile.`=>没有rabbitmq的权限=>登录`http://192.168.56.11:15672`,查看是否有openstack这个用户,没有的话创建一个并赋予权限,然后在web中添加administrator标签(具体方法参考该文档上面内容)
    ```
    systemctl enable libvirtd openstack-nova-compute
    systemctl start libvirtd openstack-nova-compute
    systemctl status libvirtd openstack-nova-compute
    ```
    6. 验证
    ```
    ######################################
    # Controller
    ######################################
    openstack host list # 测试和compute是否连接正常,如果注册过来的话这里就会显示"compute nova"这个行
    vim /var/log/nova/nova-compute.log # 如果没有注册过来看日志
    openstack image list # 测试和glance是否连接正常
    openstack endpoint list # 测试和keystone是否连接正常
    
    ```


10. Neutron控制节点 > [第三方文档](https://blog.csdn.net/LL_JCB/article/details/80193734)(需要在新窗口打开,否则502错误)+[官方文档](https://docs.openstack.org/neutron/queens/install/controller-install-rdo.html)+官方文档下面的蓝点(里面包含网络的配置文件)
+ 所有的操作都在控制节点Controller里完成
    1. 准备数据库和数据库账户
    
    ```
    ######################################################
    # Controller
    ######################################################
    mysql -u root -p
    #########################################
    CREATE DATABASE neutron;
    GRANT ALL PRIVILEGES ON neutron.* TO 'neutron'@'localhost' \
      IDENTIFIED BY 'neutron';
    GRANT ALL PRIVILEGES ON neutron.* TO 'neutron'@'%' \
      IDENTIFIED BY 'neutron';
    #########################################
    ```
    
    2. 连接keystone
        1. 使用admin账号
        ```
        source ~/admin-openrc.sh    
        ```
        2. 创建keystone的user,并加入到admin的角色里
        ```
        openstack user create --domain default --password=neutron neutron
        openstack role add --project service --user neutron admin        
        ```
        3. 注册服务
        ```
        openstack service create --name neutron \
          --description "OpenStack Networking" network

        openstack endpoint create --region RegionOne \
          network public http://controller:9696

        openstack endpoint create --region RegionOne \
          network internal http://controller:9696

        openstack endpoint create --region RegionOne \
          network admin http://controller:9696
        ```
    3. 安装依赖包
    ```

    yum install openstack-neutron openstack-neutron-ml2 \
      openstack-neutron-linuxbridge ebtables -y
    ```
    4. 修改内核参数和配置文件
        1. 确认内核支持网桥filters并作如下设置，编辑/etc/sysctl.conf增加以下内容
        ```
        vim /etc/sysctl.conf
        ######################################################
        net.bridge.bridge-nf-call-iptables=1
        net.bridge.bridge-nf-call-ip6tables=1
        ######################################################
        ```
        2. 载入br_netfilter模块
        ```
        modprobe br_netfilter
        ```
        3. 从配置文件加载内核参数
        ```
        sysctl -p
        ```

        4. 修改配置文件
            1. `/etc/neutron/neutron.conf`
            ```
            vim /etc/neutron/neutron.conf
            ############################################ok
            [database]
            connection = mysql+pymysql://neutron:neutron@controller/neutron
            ...

            [DEFAULT]
            core_plugin = ml2
            #service_plugins = router
            service_plugins = 
            #allow_overlapping_ips = true
            transport_url = rabbit://openstack:openstack@controller
            auth_strategy = keystone
            notify_nova_on_port_status_changes = true
            notify_nova_on_port_data_changes = true
            ...

            [keystone_authtoken]
            auth_uri = http://controller:5000
            auth_url = http://controller:35357
            memcached_servers = controller:11211
            auth_type = password
            project_domain_name = default
            user_domain_name = default
            project_name = service
            username = neutron
            password = neutron
            ...

            [nova]
            auth_url = http://controller:35357
            auth_type = password
            project_domain_name = default
            user_domain_name = default
            region_name = RegionOne
            project_name = service
            username = nova
            password = nova
            ...

            [oslo_concurrency]
            lock_path = /var/lib/neutron/tmp
            ...
            ############################################ok
            ```
            2. `/etc/neutron/plugins/ml2/ml2_conf.ini`
            ```
            vim /etc/neutron/plugins/ml2/ml2_conf.ini
            ############################################ok
            [ml2]
            type_drivers = flat,vlan
            tenant_network_types =  # 他人文档里是vxlan
            #mechanism_drivers = linuxbridge,l2population
            mechanism_drivers = linuxbridge
            extension_drivers = port_security
            ...

            [ml2_type_flat]
            flat_networks = provider
            ...

            #[ml2_type_vxlan]
            #vni_ranges = 1:1000
            ...

            [securitygroup]
            enable_ipset = true
            ...
            ############################################ok
            ```
            3. `/etc/neutron/plugins/ml2/linuxbridge_agent.ini`
            ```
            vim /etc/neutron/plugins/ml2/linuxbridge_agent.ini
            ############################################ok
            [linux_bridge]
            #physical_interface_mappings = provider:enp0s8   #第二张网卡网卡名
            physical_interface_mappings = provider:eth0 # 官网(eth0是我自己换的)=>我的选择
            #physical_interface_mappings = physnet1:eth0 # 老师
            ...

            [vxlan]
            enable_vxlan = false
            ...

            [securitygroup]
            enable_security_group = true
            firewall_driver = neutron.agent.linux.iptables_firewall.IptablesFirewallDriver

            ...
            ############################################ok
            ```
            4. `/etc/neutron/dhcp_agent.ini `

            ```
            vim /etc/neutron/dhcp_agent.ini 
            ############################################ok
            [DEFAULT]
            interface_driver = linuxbridge
            dhcp_driver = neutron.agent.linux.dhcp.Dnsmasq
            enable_isolated_metadata = true
            ...
            ############################################ok
            ```
            
            5. `/etc/neutron/metadata_agent.ini`
            ```
            vim /etc/neutron/metadata_agent.ini
            ############################################ok
            [DEFAULT]
            nova_metadata_host = controller
            metadata_proxy_shared_secret = neutron # METADATA_SECRET的密码
            ...
            ############################################ok
            ```
            
            6. `/etc/nova/nova.conf`
            ```
            vim /etc/nova/nova.conf
            ############################################ok
            [neutron]
            url = http://controller:9696
            auth_url = http://controller:35357
            auth_type = password
            project_domain_name = default
            user_domain_name = default
            region_name = RegionOne
            project_name = service
            username = neutron
            password = neutron
            service_metadata_proxy = true
            metadata_proxy_shared_secret = neutron
            ...
            ############################################ok
            
            
            
            
            ```
            7. `/etc/neutron/l3_agent.ini`
            ```
            vim /etc/neutron/l3_agent.ini
            #################################################
            [DEFAULT]
            interface_driver = linuxbridge
            ...
            #################################################
            ```
            
            
    5. 创建软连接
    ```
    ln -s /etc/neutron/plugins/ml2/ml2_conf.ini /etc/neutron/plugin.ini
    ```
    
    6. 为neutron导入数据库
    ```
    source ~/admin-openrc.sh    


    su -s /bin/sh -c "neutron-db-manage --config-file /etc/neutron/neutron.conf \
      --config-file /etc/neutron/plugins/ml2/ml2_conf.ini upgrade head" neutron

    mysql -uneutron -pneutron -e "use neutron;show tables;"
    ```
    
    7. 启动服务并设置开机自启动
    ```
    systemctl restart openstack-nova-api.service
    systemctl status openstack-nova-api.service
    systemctl enable neutron-server.service \
      neutron-linuxbridge-agent.service neutron-dhcp-agent.service \
      neutron-metadata-agent.service
    systemctl start neutron-server.service \
      neutron-linuxbridge-agent.service neutron-dhcp-agent.service \
      neutron-metadata-agent.service
    systemctl status neutron-server.service \
      neutron-linuxbridge-agent.service neutron-dhcp-agent.service \
      neutron-metadata-agent.service

    
    systemctl enable neutron-l3-agent.service
    systemctl start neutron-l3-agent.service
    systemctl status neutron-l3-agent.service
    ```
    
    8. 验证
    ```
    neutron agent-list # 出来结果就说明安装成功
    ```
    
    
    
10. Neutron计算节点 > [第三方文档](https://blog.csdn.net/LL_JCB/article/details/80193734)(需要在新窗口打开,否则502错误)+[官方文档](https://docs.openstack.org/neutron/queens/install/compute-install-rdo.html)+官方文档下面的蓝点(里面包含网络的配置文件)
    1. compute安装依赖包
    ```
    ################################################################
    # Compute
    ################################################################
    yum install openstack-neutron-linuxbridge ebtables ipset -y
    ```
    2. 修改compute的配置文件
    + 所有操作包括nova都在compute节点上进行
    ```
    vim /etc/neutron/neutron.conf
    #########################################################ok
    [DEFAULT]
    transport_url = rabbit://openstack:openstack@192.168.56.11
    auth_strategy = keystone
    ...

    [keystone_authtoken]
    auth_uri = http://192.168.56.11:5000
    auth_url = http://192.168.56.11:35357
    memcached_servers = 192.168.56.11:11211
    auth_type = password
    project_domain_name = default
    user_domain_name = default
    project_name = service
    username = neutron
    password = neutron
    ...

    [oslo_concurrency]
    lock_path = /var/lib/neutron/tmp
    ...
    #########################################################


    vim /etc/neutron/plugins/ml2/linuxbridge_agent.ini
    #########################################################
    [linux_bridge]
    physical_interface_mappings = provider:eth0  #第二张网卡名
    ...

    [vxlan]
    enable_vxlan = false
    ...

    [securitygroup]
    enable_security_group = true
    firewall_driver = neutron.agent.linux.iptables_firewall.IptablesFirewallDriver
    ...
    #########################################################


    vim /etc/nova/nova.conf
    #########################################################ok
    [neutron]
    url = http://192.168.56.11:9696
    auth_url = http://192.168.56.11:35357
    auth_type = password
    project_domain_name = default
    user_domain_name = default
    region_name = RegionOne
    project_name = service
    username = neutron
    password = neutron
    ...
    #########################################################
    ```
    3. 修改内核参数并加载内核模块
    ```
    vim /etc/sysctl.conf
    #########################################################
    net.bridge.bridge-nf-call-iptables=1
    net.bridge.bridge-nf-call-ip6tables=1
    #########################################################
    modprobe br_netfilter
    sysctl -p
    ```
    4. 启动服务
    ```
    systemctl restart openstack-nova-compute.service
    systemctl status openstack-nova-compute.service

    systemctl enable neutron-linuxbridge-agent.service
    systemctl start neutron-linuxbridge-agent.service
    systemctl status neutron-linuxbridge-agent.service
    ```
    
    5. 验证
    + 如果发现没有compute节点的linux bridge agent的话,查看compute和controller的防火墙和selinux是否都关闭
    + 我第一次做的时候compute节点的linux bridge agent没有.然后通过CPU占用率高和`l3-agent.log`日志文件发现l3 agent配置文件没配置,配置好后还是没有出现.所以我就看了下防火墙和selinux,然后发现compute的防火墙和selinux没有关闭,controller的selinux没有关闭.把这三个关闭后就出现compute节点了!!
    ```

    #=============================================================
    # Compute
    ################################################################
    source ~/admin-openrc.sh

    openstack extension list --network # 会输出很多
    openstack network agent list # 如果出现linux bridge agent并且节点是compute,就说明neutron安装好了
    ```




11. 创建1个实例
> [官方provider网络文档](https://docs.openstack.org/install-guide/launch-instance-networks-provider.html)
> [官方创建实例](https://docs.openstack.org/install-guide/launch-instance.html#create-virtual-networks)
```
############################################################
# Controller
############################################################
source admin-openrc.sh

grep physical_interface_mappings /etc/neutron/plugins/ml2/linuxbridge_agent.ini
#######################
physical_interfacae_mappings = physnet1:eth0 #physnet1是随便设的
#######################

# 创建网络


#-------------------------------------------------------------------
#neutron net-create flat --shared \
#  --provider:physical_network physnet1 \
#  --provider:network_type flat
#-------------------------------------------------------------------

openstack network create  --share --external \
  --provider-physical-network provider \
  --provider-network-type flat provider # 可能不应该创建external的.provider应该是网络名把




# 创建子网
#-------------------------------------------------------------------
#neutron subnet-create flat 192.168.56.0/24 --name flat-subnet \
#  --alocation-pool start=192.168.56.100,end=192.168.56.200
#  --dns-nameserver 192.168.56.2 --gateway 192.168.56.2  
#-------------------------------------------------------------------
  
  
openstack subnet create --network provider \
  --allocation-pool start=192.168.56.100,end=192.168.56.200 \
  --dns-nameserver 8.8.8.8 --gateway 192.168.56.2 \
  --subnet-range 192.168.56.0/24 provider

neutron net-list # DHCP一定要关闭(菜单栏的NAT那里不要勾选DHCP就可以了)



# 自己创建一个flavour(虚拟机的配置,包括内存大小啊,磁盘等)
source ~/admin-openrc.sh
openstack flavor create --id 0 --vcpus 1 --ram 64 --disk 1 m1.nano # 需要admin身份

source ~/demo-openrc.sh
ssh-keygen -q -N "" # 需要完整输入/root/.ssh/id_rsa确认
ls .ssh/ # 出现了就行
openstack keypair create --public-key ~/.ssh/id_rsa.pub mykey 
openstack keypair list

openstack security group rule create --proto icmp default
openstack security group rule create --proto tcp --dst-port 22 default
openstack flavor list

openstack image list
openstack network list
openstack security group list
openstack keypair list

openstack server create --flavor m1.nano --image cirros \
  --nic net-id=8846d656-c107-4216-9fc9-22402f49484b --security-group default \
  --key-name mykey provider-instance


openstack server list

openstack console url show provider-instance

#-----------------------------------------------------------
source ~/demo-openrc.sh
ssh-keygen -q -N ""
ls .ssh/
# 创建虚拟机的时候,把公钥放在虚拟机里面=>可以直接连接了
nova keypair-add --pub-key .ssh/id_rsa.pub mykey
nova keypair-list # 列出来就没问题了

# 防火墙,安全组默认只有的default
nova secgroup-add-rule default icmp -1 -1 0.0.0.0/0
nova secgroup-add-rule default tcp 22 22 0.0.0.0/0

# 查看有哪些虚拟机
nova flavor-list # 选个最小的虚拟机

nova image-list # 需要什么镜像

neutron net-list # 需要什么网络
# 配置 镜像 网络 安全组 钥匙对 名称
nova boot --flavor ml.tiny --image cirrors \
  --nic net-id=b997cxxxx --security-group default \
  --key-name mykey hello-instance # 这些flavour,image,id什么的都是看上面的.hello-instance

nova list # 创建出来就是状态成功了=>status如果是active就好了
# ping 下这个网络就好了
ssh cirros@192.168.56.101 # 就可以连接了
############################################################
ifconfig # controller和新虚拟机上都可以尝试
############################################################

nova get-vnc-console hello-instance novnc 
# 通过url就去访问

```

12. 安装dashboard > [第三方文档](https://blog.csdn.net/LL_JCB/article/details/80221646)
    1. 安装软件包
    ```
    yum install openstack-dashboard -y
    ```
    
    2. 编辑`/etc/openstack-dashboard/local_settings`文件完成如下配置
    ```
    vim /etc/openstack-dashboard/local_settings
    ############################################################################################
    ...
    OPENSTACK_HOST = "controller"   #配置界面在控制节点使用
    ...
    ALLOWED_HOSTS = ['*']           #允许所有主机访问
    ...
    SESSION_ENGINE = 'django.contrib.sessions.backends.cache'   #配置memcached存储服务
    ...
    CACHES = {
        'default': {
            'BACKEND': 'django.core.cache.backends.memcached.MemcachedCache',
            'LOCATION': 'controller:11211',
        },
    }
    ...
    OPENSTACK_KEYSTONE_URL = "http://%s:5000/v3" % OPENSTACK_HOST  #启动v3的认证api
    ...
    OPENSTACK_KEYSTONE_MULTIDOMAIN_SUPPORT = True  #启用domain支持
    ...
    OPENSTACK_API_VERSIONS = {     #配置api版本
        "identity": 3,
        "image": 2,
        "volume": 2,
    }
    ...
    OPENSTACK_KEYSTONE_DEFAULT_DOMAIN = 'Default'  #配置default为默认域
    ...
    OPENSTACK_KEYSTONE_DEFAULT_ROLE = "user"       #配置user角色为默认角色
    ...
    TIME_ZONE = "Asia/Shanghai"   #设置时区
    ############################################################################################
    ```
    3. 重启httpd和memcached服务
    ```
    systemctl restart httpd.service memcached.service
    ```
    
    4. 验证
        1. 在浏览器输入http://192.168.0.77/dashboard
        2. 使用admin用户或者demo用户登录，default作为默认域
        + admin:ADMIN_PASS demo:demo

13. OpenStack最终配置情况如下:
```
[root@controller ~]# openstack flavor list
+----+---------+-----+------+-----------+-------+-----------+
| ID | Name    | RAM | Disk | Ephemeral | VCPUs | Is Public |
+----+---------+-----+------+-----------+-------+-----------+
| 0  | m1.nano |  64 |    1 |         0 |     1 | True      |
+----+---------+-----+------+-----------+-------+-----------+
[root@controller ~]# openstack image list
+--------------------------------------+--------+--------+
| ID                                   | Name   | Status |
+--------------------------------------+--------+--------+
| fa5f5dcb-6c1d-4d6b-8550-bdcc95a2df60 | cirros | active |
+--------------------------------------+--------+--------+
[root@controller ~]# openstack network list
+--------------------------------------+----------+--------------------------------------+
| ID                                   | Name     | Subnets                              |
+--------------------------------------+----------+--------------------------------------+
| 8846d656-c107-4216-9fc9-22402f49484b | provider | 2efa14c6-ce85-4188-b6c8-47e323da638f |
+--------------------------------------+----------+--------------------------------------+
[root@controller ~]# openstack security group list
+--------------------------------------+---------+------------------------+----------------------------------+
| ID                                   | Name    | Description            | Project                          |
+--------------------------------------+---------+------------------------+----------------------------------+
| 31a4714b-a9c5-41a7-a446-56e28ca3a5e9 | default | Default security group | 00dba441e74d4f02815ed3076142d5c8 |
+--------------------------------------+---------+------------------------+----------------------------------+
[root@controller ~]# openstack keypair list
+-------+-------------------------------------------------+
| Name  | Fingerprint                                     |
+-------+-------------------------------------------------+
| mykey | d0:3c:33:47:62:e3:87:d2:46:3d:44:78:8b:17:d6:6d |
+-------+-------------------------------------------------+
[root@controller ~]# openstack server create --flavor m1.nano --image cirros \
>   --nic net-id=8846d656-c107-4216-9fc9-22402f49484b --security-group default \
>   --key-name mykey provider-instance
+-----------------------------+-----------------------------------------------+
| Field                       | Value                                         |
+-----------------------------+-----------------------------------------------+
| OS-DCF:diskConfig           | MANUAL                                        |
| OS-EXT-AZ:availability_zone |                                               |
| OS-EXT-STS:power_state      | NOSTATE                                       |
| OS-EXT-STS:task_state       | scheduling                                    |
| OS-EXT-STS:vm_state         | building                                      |
| OS-SRV-USG:launched_at      | None                                          |
| OS-SRV-USG:terminated_at    | None                                          |
| accessIPv4                  |                                               |
| accessIPv6                  |                                               |
| addresses                   |                                               |
| adminPass                   | dnvBf3n97Q3Z                                  |
| config_drive                |                                               |
| created                     | 2018-08-05T06:13:06Z                          |
| flavor                      | m1.nano (0)                                   |
| hostId                      |                                               |
| id                          | c6698a7b-e2e9-4de9-91de-3c6b2b4c0531          |
| image                       | cirros (fa5f5dcb-6c1d-4d6b-8550-bdcc95a2df60) |
| key_name                    | mykey                                         |
| name                        | provider-instance                             |
| progress                    | 0                                             |
| project_id                  | 00dba441e74d4f02815ed3076142d5c8              |
| properties                  |                                               |
| security_groups             | name='31a4714b-a9c5-41a7-a446-56e28ca3a5e9'   |
| status                      | BUILD                                         |
| updated                     | 2018-08-05T06:13:06Z                          |
| user_id                     | 61db932328f844b8b5f1b3a8d38bfd2f              |
| volumes_attached            |                                               |
+-----------------------------+-----------------------------------------------+

[root@controller nova]# openstack server list
+--------------------------------------+-------------------+--------+----------+--------+---------+
| ID                                   | Name              | Status | Networks | Image  | Flavor  |
+--------------------------------------+-------------------+--------+----------+--------+---------+
| c6698a7b-e2e9-4de9-91de-3c6b2b4c0531 | provider-instance | ERROR  |          | cirros | m1.nano |
+--------------------------------------+-------------------+--------+----------+--------+---------+


[root@controller nova]# openstack image show fa5f5dcb-6c1d-4d6b-8550-bdcc95a2df60
+------------------+------------------------------------------------------+
| Field            | Value                                                |
+------------------+------------------------------------------------------+
| checksum         | 443b7623e27ecf03dc9e01ee93f67afe                     |
| container_format | bare                                                 |
| created_at       | 2018-08-04T09:41:29Z                                 |
| disk_format      | qcow2                                                |
| file             | /v2/images/fa5f5dcb-6c1d-4d6b-8550-bdcc95a2df60/file |
| id               | fa5f5dcb-6c1d-4d6b-8550-bdcc95a2df60                 |
| min_disk         | 0                                                    |
| min_ram          | 0                                                    |
| name             | cirros                                               |
| owner            | e471412bd08b4b5292066e4cf5e7e45b                     |
| protected        | False                                                |
| schema           | /v2/schemas/image                                    |
| size             | 12716032                                             |
| status           | active                                               |
| tags             |                                                      |
| updated_at       | 2018-08-04T09:41:29Z                                 |
| virtual_size     | None                                                 |
| visibility       | public                                               |
+------------------+------------------------------------------------------+



```



## 3. 问题
创建出来的虚拟机是ERROR状态
1. 同步时间
+ 子节点通过暂停chronyd,然后变成正确时间后再打开服务同步了,但还是没用...

2. 配置不符合镜像要求


3. compute要有可用资源 
4.查看所有服务是不是都是正常的

5. 安装dashboard,然后启动/骚操作实例,然后会有报错
+ 我的报错是:`Unable to connect to Neutron.`
    1. 使用`openstack network agent list`,发现compute节点的alive是"XXX"
        1. 根据[这篇文档](https://blog.csdn.net/controllerha/article/details/78837637)
        + compute节点和controller节点时间可能不同步
        + eht0网卡可能申明不正确=>都对的
        2. 我重启了compute的`systemctl restart neutron-linuxbridge-agent.service`,发现变成`:-)`,ok了,但是server还是ERROR状态
    2. 在dashoard的instances下的Overview下的Fault这里,我看到`Host 'compute' is not mapped to any cell`        
        1. 根据[这个网站](https://www.leolan.top/index.php/posts/209.html),我执行了`source ~/admin-openrc.sh && su -s /bin/sh -c "nova-manage cell_v2 discover_hosts --verbose" nova && nova-manage cell_v2 discover_hosts`后,创建新的server,结果显示BUILD,但还是无法连接Neutron
        2. 终于发现原来我之前遗漏步骤了,就是[nova-compute官方文档](https://docs.openstack.org/nova/queens/install/compute-install-rdo.html)的virt_type和下面的添加计算节点都忘记执行了.
    3. 在compute节点的neutron日志文件中,发现错误`Unserializable message: ('#ERROR', ValueError('I/O operation on closed file',))`
        1. [这个网站](https://www.cnblogs.com/yaohong/p/7719357.html)说这个错误不影响使用..=>所以我没有理会这个错误
    4. 过了一段时间后,刚才创建的server挂了,从BUILD变成ERROR,然后看错误显示说`Failed to allocate the network(s), not rescheduling.`
        1. 根据[这个网站](https://blog.csdn.net/sfdst/article/details/70809935),我修改了配置文件并重启服务,再删除原来的服务后再用3.4版本的镜像重新创建一个后就ACTIVE了!!
        ```
        vim /etc/nova/nova.conf
        ###########################################################################################
        #Fail instance boot if vif plugging fails  
        vif_plugging_is_fatal = False  

        #Number of seconds to wait for neutron vif  
        #plugging events to arrive before continuing or failing  
        #(see vif_plugging_is_fatal). If this is set to zero and  
        #vif_plugging_is_fatal is False, events should not be expected to arrive at all.  
        vif_plugging_timeout = 0  
        ###########################################################################################
        systemctl restart openstack-nova-compute.service
        ```
    5. 登录url后发现`stuck in ""Booting from Hard Disk ... GRUB"`
        1. 根据[这个网站](https://github.com/AJNOURI/COA/issues/50),可能是我的virt_type=kvm不太好,所以可以考虑改成qemu=>改成了qemu后,确实跳过`GRUB`了,但是卡在`further output written to /dev/ttyS0`了
    6. 通过URL的VNC登录虚拟机发现卡在`further output written to /dev/ttyS0`
        1. 这不是问题.过段时间就好了,并且能登录了!!根据提示的用户名和密码登录
        + 我这边显示的用户名是`cirros`,密码是`cubswin:)`(包括后面的表情)
    7. 上面步骤创建的虚拟机没有网络...ping不通也ssh不通
6. `More than one SecurityGroup exists with the name 'default'.`
+ 需要切换到demo用户才能创建server

