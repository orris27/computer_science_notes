## 1. 代码简单说明
1. msg-create:创建key为1234的消息队列
2. msg-rm:删除key为1234的消息队列
3. msg-stat:显示key为1234的消息队列的状态
4. msg-set:设置key为1234的消息队列的状态
5. msg-snd:发送消息给key为1234的消息队列
6. msg-rcv:接收来自key为1234的消息队列的消息

## 2. 发送与接收消息
```
./msg-snd 100 1
./msg-snd 200 2
./msg-snd 300 3
./msg-snd 400 4
./msg-rcv -t -3 # 接收消息类型为3的消息.msg-rcv的参数中"-n"表示NOWAIT,而"-t <val>"表示接收消息类型为<val>的消息
```
