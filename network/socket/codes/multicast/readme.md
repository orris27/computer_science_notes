## 1. UDP组播实现
1. 思路
    1. 服务端开放组播权限
        1. 组播地址
            1. `239.0.0.0～239.255.255.255` :使用本地管理组播地址，仅在特定的本地范围内有效。(实验中我们使用了`239.0.0.2`)
        2. 自己的ip("0.0.0.0"交给系统去选择)
        3. 网卡编号:eth0的编号
    2. 客户端加入到组播中
    3. 服务端给组播地址发送消息,客户端就能收到了
2. 注意
    1. 服务端的组播地址=客户端的组播地址(我感觉是因为这个组播地址应该表示的是和广播地址类似的含义)

```
./server 
#---------------------------------------------------
# send msg
# send msg
# send msg
#---------------------------------------------------

./client 
#---------------------------------------------------
# waiting...
# waiting finished
# hellowaiting...
# waiting finished
# hellowaiting...
# waiting finished
# hellowaiting...
#---------------------------------------------------
```