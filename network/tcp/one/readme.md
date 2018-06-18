客户端如果使用Ctrl+C结束进程的话,对于服务器来说是能够收到她关闭socket的消息的.所以客户端不用写try块.而服务器其实也不用写try块...
服务器的```s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)```是为了保证服务器先于客户端关掉时,会处于TIME_WAIT状态2MSL时间.这段时间内端口号不释放.因此如果不写这句话,就不能用该端口.
```
python server.py
python client.py
```
