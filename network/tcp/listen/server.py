'''
    Create a TCP server that can only accept 5 people
'''
import socket

s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
my_addr=('127.0.0.1',2333)
s.bind(my_addr)

listen_num=int(input('Listen num:'))
s.listen(listen_num)

import time
import sys
for i in range(10):
    sys.stdout.write('\r%d seconds'%(i))
    sys.stdout.flush()
    time.sleep(1)

print('')
while True:
    client_socket,client_addr=s.accept()
    print("Accepted %s"%(str(client_addr)))
    time.sleep(1)


