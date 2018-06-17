import socket
import sys

if len(sys.argv) != 3:
    print('-'*30)
    print('tips:')
    print('python xxx.py 10.189.190.100 2334')
    print('-'*30)
    exit()

else:
    curr_ip=sys.argv[1]
    curr_port=int(sys.argv[2])



s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)


s.bind((curr_ip,curr_port))
#print(s.getsockname())

data=s.recvfrom(1024) # data is a 2d tuple. (raw_data,src_ip)

print(data)

print(data[0].decode('utf-8'))

s.close()
