'''
    Create a server that 1)can only serve at most one person at the same time 2)print the message from the client 3)send back a "thank you" message
    1. create a socket
    2. enable the socket to bind a same port
    3. bind
    4. set blocking to false
    5. listen
    6. begin to accept forever 
    7.  If nobody comes, throw the exception
    8.  Else somebody comes, set the client not blocking and to the list
    9. traverse all the clients and revoke their "recv" function
    10.  If no, throw the exception
    11.  Else, deal with the message(if lenght is 0,close it and remove it from the list) 
'''
import socket

with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
    
    s.setblocking(False)

    my_addr=('127.0.0.1',2333)
    s.bind(my_addr)

    s.listen(5)

    client_list=[]

    while True:

        try:
            client_socket,client_addr=s.accept()
        except:
            pass
        else:
            print('A new client was connected.')
            client_socket.setblocking(False)
            client_list.append((client_socket,client_addr))
        
        for client_socket,client_addr in client_list:
            try:
                received_data=client_socket.recv(1024)
            except:
                pass
            else:
                if len(received_data) == 0: # client closes his/her socket
                    client_socket.close()
                    client_list.remove((client_socket,client_addr))
                else:
                    print("%s:%s"%(str(client_addr),received_data.decode('utf-8')))

            
