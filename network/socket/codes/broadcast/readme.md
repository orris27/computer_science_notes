## 1. UDP实现广播
1. 注意
    1. 服务端  
        1. 允许套接字发送广播
        2. 发送的端口是3000
    2. 客户端
        1. 绑定的ip是0.0.0.0:3000.不能写真实IP地址
```
./server 
#---------------------------
# send msg
# send msg
# send msg
#---------------------------


./client 
#---------------------------
# waiting...
# waiting finished
# hellowaiting...
# waiting finished
#---------------------------

```
