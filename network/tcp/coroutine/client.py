'''
    A client that connect to the server and send the user input
    1. create a socket
    2. connect to the server
    3. wait for user input
    4. send the user input to the data
'''

import socket

with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:


    server_addr=('127.0.0.1',2333)

    s.connect(server_addr)

    while True:
        message=input('Your message:')
        s.send(message.encode('utf-8'))
        
        server_response=s.recv(1024)
        print(server_response.decode('utf-8'))

