'''
    Connect the server 20 times

'''
import socket

server_addr=('127.0.0.1',2333)



connect_num=int(input("Connect num:"))
for i in range(connect_num):
    s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.connect(server_addr)
    print('%d:connect successfully!'%(i))

