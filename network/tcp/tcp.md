## 1. 数据包
### 1-1. 结构
1. [TCP数据包结构中文](https://img-blog.csdn.net/20130629171810234?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvZnp6bW91c2U=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center) + [TCP数据包结构英文](https://img-blog.csdn.net/20130629171829718?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvZnp6bW91c2U=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center)

2. 说明
    1. 16位源端口号
    2. 16位目的端口号
    3. 32位序列号
    4. 32位确认号
    5. 4位首部长度
    6. 6位保留
    7. 6位标志位
        1. URG：紧急指针有效
        2. ACK：确认序号有效
        3. PSH：接收方尽快将这个报文段交给应用层
        4. RST：重建连接
        5. SYN：同步序号用来发起一个连接.标志位置1
        6. FIN：发端完成发送任务
    8. 16位窗口大小
    9. 16位TCP检验和
    10. 16位紧急指针
    11. 选项(若有)
    12. 数据(若有)


