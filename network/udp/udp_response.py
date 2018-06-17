import socket

curr_ip='10.189.177.245'

s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

s.bind((curr_ip,2334))

data=s.recvfrom(1024) # data is a 2d tuple. (raw_data,src_ip)

print(data)

print(data[0].decode('utf-8'))
