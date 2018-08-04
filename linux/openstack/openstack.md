## 1. 安装
+ 整个安装参考[他人文档](http://blog.51cto.com/andyliu/2154044)+[官方文档](https://docs.openstack.org/keystone/queens/install/keystone-install-rdo.html)
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
    wget http://download.cirrors-cloud.net/0.3.4-x84_64-disk.img
    
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
      IDENTIFIED BY 'NOVA_DBPASS';
    GRANT ALL PRIVILEGES ON nova_api.* TO 'nova'@'%' \
      IDENTIFIED BY 'NOVA_DBPASS';

    GRANT ALL PRIVILEGES ON nova.* TO 'nova'@'localhost' \
      IDENTIFIED BY 'NOVA_DBPASS';
    GRANT ALL PRIVILEGES ON nova.* TO 'nova'@'%' \
      IDENTIFIED BY 'NOVA_DBPASS';

    GRANT ALL PRIVILEGES ON nova_cell0.* TO 'nova'@'localhost' \
      IDENTIFIED BY 'NOVA_DBPASS';
    GRANT ALL PRIVILEGES ON nova_cell0.* TO 'nova'@'%' \
      IDENTIFIED BY 'NOVA_DBPASS';
    
    
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









## 2. 安装(失败品,根据老师的L版操作的)
+ 失败的原因是在`/etc/keystone/keystone.conf`中让token和revoke使用了`memcache`,结果就导致了`Failed to discover available identity versions when contacting http://controller:35357/v3. Attempting to parse version from URL.Internal Server Error (HTTP 500)`错误
+ 使用`/etc/keystone/keystone.conf`的话,要重启httpd
1. 环境准备
    1. CentOS-7.1 系统2台,每台2G内存
    + linux-node1.oldboyedu.com 192.168.56.11 网卡NAT eth0 控制节点
    + linux-node2.oldboyedu.com 192.168.56.12 网卡NAT eth0 计算节点
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
    192.168.56.11 linux-node1 linux-node1.oldboyedu.com
    192.168.56.12 linux-node2 linux-node2.oldboyedu.com
    EOF
    ```
    3. 硬盘50G
    
    4. Tightvnc view VNC客户端
    
    5. 安装包:老师发给大家
    
    6. 理解SOA,RestAPI,消息队列,对象存储
    
    7. 使用L版OpenStack


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

timedatectl set-timezone Asia/Shanghai
date # 和当前的时间一样
```

3. 安装数据库
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
create database keystone;
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
    5. 浏览器访问`192.168.56.11:15672`(Web管理页面)
    + 默认的用户密码都是guest
    + Admin这里赋值admin的tags(administrator),然后点击openstack,然后在Update this user的地方,复制刚才的tags进这个的Tags并修改密码,然后update
    
    6. 浏览器访问`192.168.56.11:15672`,然后可以输入刚才的用户名和密码(如openstack:openstack)
    
    7. (可选)监听的话,使用`HTTP api`(在网页的最下面,很小的字)
    
    
    
    
    
    
5. 部署keystone(验证服务)>[官方文档](https://docs.openstack.org/keystone/queens/install/keystone-install-rdo.html)
    1. 安装OpenStack的queens版本的rpm包
    + `No+package+openstack-keystone+available.`的错误是因为没有安装足够的源
    + 以前的如juno等都不可用了,可以在下面链接里查看可用的版本
    > https://repos.fedorapeople.org/repos/openstack
    ```
    yum install yum-plugin-priorities # 如果使用阿里的yum源的话,就需要换成原来的源
    yum install -y https://repos.fedorapeople.org/repos/openstack/openstack-queens/rdo-release-queens-1.noarch.rpm

    yum install -y openstack-keystone httpd mod_wsgi memcached python-memcached
    yum install -y lrzsz
    ```
    
    
    2. 配置keystone的配置文件
    + 前面不能有空格
    + 配置数据库=>保存验证信息
        - 用户名:密码@主机/数据库名
    ```
    openssl rand -hex 10 # 随机生成一个字符串作为admin的token
    #########################
    b337e9fd9ef8eee3cf2e
    #########################

    vim /etc/keystone/keystone.conf
    #######################################
    #admin_token = ADMIN # keystone默认没有用户,这样就验证不进去,所以提供了admin的token
    admin_token = b337e9fd9ef8eee3cf2e

    [database]
    connection = mysql://keystone:keystone@192.168.56.11/keystone
    #######################################
    ```

    3. 导入数据库

    ```
    su -s /bin/sh -c "keystone-manage db_sync" keystone # 切换到keystone用户是因为让下面的日志文件的属主为keystone,这样就可以读写日志文件
    # tail /var/log/keystone/keystone.log

    mysql -h 192.168.56.11 -u keystone -pkeystone
    ######################
    use keysmtone;
    show tables; # 如果出现表就说明导入数据库成功了
    ######################
    ```
    
    4. 连接keystone和memcache
    ```
    vim /etc/keystone/keystone.conf
    ###########################################
    verbose = true # 我这里好像是#debug = false,老师讲这个说是为了让日志级别=info=>好排除问题
    
    [memcache]
    servers = 192.168.56.11:11211 # 我这里只有:memcache_servers = 192.168.56.11:11211
    
    [token]
    provider = uuid # 指定
    driver = memcache # 指定token写在memcache里,而非MySQL
    
    [revoke] # 回滚
    driver = sql
    ###########################################
    grep '^[a-Z]' /etc/keystone/keystone.conf 
    #######################################################################
    admin_token = b337e9fd9ef8eee3cf2e
    memcache_servers = 192.168.56.11:11211
    ### 三个#表示是老师的操作,而下一行是官方文档中Queen的操作
    ###connection = mysql://keystone:keystone@192.168.56.11/keystone
    connection = mysql+pymysql://keystone:keystone@192.168.56.11/keystone
    driver = sql
    provider = uuid
    driver = memcache
    #######################################################################
    ```
    5. 启动keystone
    ```
    systemctl start memcached.service
    
    
    ############################################################################
    # 老师的做法 starts
    ############################################################################
    vim /etc/httpd/conf.d/wsgi-keystone.conf
    ############################################################
    Listen 5000
    Listen 35357

    <VirtualHost *:5000>
        WSGIDaemonProcess keystone-public processes=5 threads=1 user=keystone group=keystone display-name=%{GROUP}
        WSGIProcessGroup keystone-public
        WSGIScriptAlias / /usr/bin/keystone-wsgi-publi
        WSGIApplicationGroup %{GLOBAL}
        WSGIPassAuthorization On
        <IfVersion >= 2.4>
            ErrorLogFormat "%{cu}t %M"
        </IfVersion>

        ErrorLog /var/log/httpd/keystone-error.log
        CustomLog /var/log/httpd/keystone-access.log combined

        <Directory /usr/bin>
            <IfVersion >= 2.4>
                Require all granted
            </IfVersion>
            <IfVersion < 2.4>
                Order allow,deny
                Allow from all
            </IfVersion>
        </Directory>
    </VirtualHost>


    <VirtualHost *:35537>
        WSGIDaemonProcess keystone-admin processes=5 threads=1 user=keystone group=keystone display-name=%{GROUP}
        WSGIProcessGroup keystone-admin
        WSGIScriptAlias / /usr/bin/keystone-wsgi-admin
        WSGIApplicationGroup %{GLOBAL}
        WSGIPassAuthorization On

        <IfVersion >= 2.4>
            ErrorLogFormat "%{cu}t %M"
        </IfVersion>

        ErrorLog /var/log/httpd/keystone-error.log
        CustomLog /var/log/httpd/keystone-access.log combined

        <Directory /usr/bin>
            <IfVersion >= 2.4>
                Require all granted
            </IfVersion>
            <IfVersion < 2.4>
                Order allow,deny
                Allow from all
            </IfVersion>
        </Directory>
    </VirtualHost>
    ############################################################
    ############################################################################
    # 老师的做法 ends
    ############################################################################




    ############################################################################
    # 官方的做法starts
    ############################################################################
    ln -s /usr/share/keystone/wsgi-keystone.conf /etc/httpd/conf.d/
    ############################################################################
    # 官方的做法ends
    ############################################################################





    vim /etc/httpd/conf/httpd.conf
    ############################################################
    ServerName 192.168.56.11:80 # 如果不启动这个,OpenStack就启动不了
    ############################################################
    systemctl enable memcached
    systemctl enable httpd
    systemctl start httpd
    ```

    6. 验证
    + 如果启动了下面的端口就说明正常
    + 如果缺少35357和5000的话,可以重启下httpd(我第一次时就遇到这个情况)
    ```
    netstat -lntup | grep httpd 
    #################################################################################################
    tcp6       0      0 :::35357                :::*                    LISTEN      5201/httpd          
    tcp6       0      0 :::5000                 :::*                    LISTEN      5201/httpd          
    tcp6       0      0 :::80                   :::*                    LISTEN      5201/httpd    
    #################################################################################################
    ```
    
    7. 登录
    + 环境变量来连接
    + 版本的好处是保持前端和后端的版本同步更新等
    ```
    #############################################################################
    # openstack --help | less => 查看最新的环境变量来登录
    #############################################################################
    export OS_TOKEN=b337e9fd9ef8eee3cf2e
    export OS_URL=http://192.168.56.11:35357/v3 # 管理员使用35357端口,版本是v3
    export OS_IDENTITY_API_VERSION=3
    
    #yum -y install python-pip
    yum install centos-release-openstack-queens -y
    #yum upgrade
    yum install python-openstackclient -y # 安装了这个后才有openstack命令
    openstack user list
    
    
    ################################################################################
    # 为了实现openstack domain 能够创造的尝试 (有人重启httpd就解决了...)
    ################################################################################
    ln -s /usr/share/keystone/wsgi-keystone.conf /etc/httpd/conf.d/ # openstack domain的错误提示变成了500
    keystone-manage fernet_setup --keystone-user keystone --keystone-group keystone
    keystone-manage credential_setup --keystone-user keystone --keystone-group keystone
    tail /var/log/httpd/keystone.log
    keystone-manage bootstrap --bootstrap-password ADMIN_PASS \
      --bootstrap-admin-url http://192.168.56.11:5000/v3/ \
      --bootstrap-internal-url http://192.168.56.11:5000/v3/ \
      --bootstrap-public-url http://192.168.56.11:5000/v3/ \
      --bootstrap-region-id RegionOne
    ######################
    [token]
    # ...
    provider = fernet
    ######################
    
    
    
    export OS_USERNAME=admin
    export OS_PASSWORD=ADMIN_PASS
    export OS_PROJECT_NAME=admin
    export OS_USER_DOMAIN_NAME=Default
    export OS_PROJECT_DOMAIN_NAME=Default
    export OS_AUTH_URL=http://192.168.56.11:35357/v3
    export OS_IDENTITY_API_VERSION=3
    
    openstack domain create default
    openstack project create --domain default --description "Admin Project" admin
    ```
    
    ```
    keystone-manage bootstrap --bootstrap-password ADMIN_PASS \
      --bootstrap-admin-url http://controller:5000/v3/ \
      --bootstrap-internal-url http://controller:5000/v3/ \
      --bootstrap-public-url http://controller:5000/v3/ \
      --bootstrap-region-id RegionOne
    ln -s /usr/share/keystone/wsgi-keystone.conf /etc/httpd/conf.d/
    
    
    export OS_USERNAME=admin
    export OS_PASSWORD=ADMIN_PASS
    export OS_PROJECT_NAME=admin
    export OS_USER_DOMAIN_NAME=Default
    export OS_PROJECT_DOMAIN_NAME=Default
    export OS_AUTH_URL=http://192.168.56.11:35357/v3
    export OS_IDENTITY_API_VERSION=3
    ```
    
    
    11. 连接上OpenStack拿token完
        1. 去除原来的环境变量(一定要去掉) 
        + 不去除的话会冲突
        + `#-------`范围内的是老师上课讲的,其余是官方文档/他人文档的,我采用的是官方文档/他人文档
        ```
        unset OS_AUTH_URL OS_PASSWORD
        #--------------------------------------
        unset OS_TOKEN
        unset OS_URL
        #--------------------------------------
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
        
        
        #-----------------------------------------------------------
        openstack --os-auth-url http://192.168.56.11:35357/v3 \
        --os-project-domain-id default \
        --os-user-domain-id default \
        --os-project-name admin \
        --os-username admin \
        --os-auth-type password \
        token issue # 输入密码admin.如果能拿到id(token)就说明keystone成功
        #-----------------------------------------------------------
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
        
        
        
        
        #-----------------------------------------------------------
        cd ~
        vim admin-openrc.sh
        ######################################################
        export OS_PROJECT_DOMAIN_ID=default
        export OS_USER_DOMAIN_ID=default
        export OS_PROJECT_NAME=admin
        export OS_TENANT_NAME=admin
        export OS_USERNAME=admin
        export OS_PASSWORD=admin # 这里为密码
        export OS_AUTH_URL=http://192.168.56.11:35357/v3
        export OS_IDENTITY_API_VERSION=3
        ######################################################
        chmod +x admin-openrc.sh
        
        source admin-openrc.sh
        openstack token issue # 这样就可以执行了
        #-----------------------------------------------------------
        ```
    
6. glance
```
yum install -y openstack-glance python-glance python-glanceclient
```

7. Nova
```
yum install -y openstack-nova-api openstack-nova-cert \
openstack-nova-conductor openstack-nova-console \
openstack-nova-novncproxy openstack-nova-scheduler 
```






