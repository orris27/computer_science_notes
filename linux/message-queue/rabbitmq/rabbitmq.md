## 1. 安装
    
启动Rabbitmq服务
rabbitmq-server启动服务后端口是5672
```
systemctl enable rabbitmq-server.service
systemctl start rabbitmq-server.service
systemctl status rabbitmq-server.service
```
添加消息队列的认证
消息队列肯定要有认证系统
netstat -lntup | grep 5672

rabbitmqctl add_user openstack openstack
rabbitmqctl set_permissions openstack ".*" ".*" ".*"
rabbitmq-plugins list
rabbitmq-plugins enable rabbitmq_management
systemctl restart rabbitmq-server.service
检查网络状态
netstat -lntup | grep 15672
