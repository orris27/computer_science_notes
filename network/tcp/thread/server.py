'''
    Create a server that 1)can only serve at most one person at the same time 2)print the message from the client 3)send back a "thank you" message
    !!Important: only the child thread can close the client_socket! Main thread cannot close client_socket
    1. create a socket
    2. enable the socket to bind a same port
    3. bind
    4. listen
    5. begin to accept forever
    6.      if ok, then receive&print message from the client until the client closed socket
    7.      close the client_socket
    8.      back to the step#5
'''
import socket
import threading

def handle_client(client_socket,client_addr):
    try:
        while True:
        
            received_data=client_socket.recv(1024)
            
            if len(received_data) >0:
                print("%s:%s"%(str(client_addr),received_data.decode('utf-8')))
                client_socket.send('Thank you!'.encode('utf-8'))

            else:
                print("service for %s stops"%(str(client_addr)))
                break
    finally:
        client_socket.close()

with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)

    my_addr=('127.0.0.1',2333)
    s.bind(my_addr)

    s.listen(5)


    while True:

        client_socket,client_addr=s.accept()
        
        t = threading.Thread(target=handle_client,args=(client_socket,client_addr))
        t.start()

