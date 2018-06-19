'''
    A HTTP server
    1. create a socket
    2. bind & reuse addr
    3. listen
    4. while True:
        4.1 accept
        4.2 new client comes=> create a new process to handle it
'''
import socket
import multiprocessing

def handle_client(client_socket,client_addr):
    '''
        A function that handles the client's request
        1. recv 
        2. deal with the message
        3. send back
    '''
    while True:
        recv_data=client_socket.recv(1024)
        with open('1.txt','wb') as f:
            f.write(recv_data)
        print(recv_data)

with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)

    s.bind(('127.0.0.1',2333))

    s.listen(100)

    while True:

        client_socket,client_addr=s.accept()
        
        p = multiprocessing.Process(target=handle_client,args=[client_socket,client_addr])
        
        p.start()

        client_socket.close()
    
    

