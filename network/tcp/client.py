'''
    Create a TCP client
    1. create a socket
    2. connect to the server
    3. send some messages to her
    4. receive message from her and print it
    5. close the socket
'''
import socket
import sys

if len(sys.argv) != 3:
    print('-'*30) 
    print('tips:')
    print('python xxx.py 10.189.190.100 2333')
    print('-'*30) 
    exit()
else:
    server_ip=sys.argv[1]
    server_port=int(sys.argv[2])

with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:

    s.connect((server_ip,server_port))
    
    while True:
        message=input("To server:")
        s.send(message.encode('utf-8'))

        received_data=s.recv(1024)

        if len(received_data) <= 0:
            break

        print("Server:%s:%s"%(str((server_ip,server_port)),received_data.decode('utf-8')))


