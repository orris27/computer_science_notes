'''
    Create a simple TCP server
    1. create a socket
    2. bind ip & port
    3. listen
    4. accept
    5. recv
    6. if I receive some data from the client, I send back 'thank you for using'
    7. then I close myself
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
    my_ip=sys.argv[1]
    my_port=int(sys.argv[2])

with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:
    s.bind((my_ip,my_port))

    s.listen(5)

    while True:
        client_socket,client_addr=s.accept()

        while True:
            received_data=client_socket.recv(1024)

            if len(received_data) <= 0:
                break
            print("Client:%s:%s"%(str(client_addr),received_data.decode('utf-8')))
            
            message=input('To client:')
            client_socket.send(message.encode('utf-8'))

        client_socket.close() 
