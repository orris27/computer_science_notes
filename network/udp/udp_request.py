import socket

curr_ip='10.189.177.245'

s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

s.bind((curr_ip,2333))

s.sendto('hello'.encode("utf-8"),(curr_ip,2334))


