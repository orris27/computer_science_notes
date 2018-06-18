'''
    Create a server that 1)can only serve at most one person at the same time 2)print the message from the client 3)send back a "thank you" message
    1. create a socket
    2. enable the socket to bind a same port
    3. bind
    4. listen
    5. create an epoll object
    6. register the server_socket to the epoll object
    7. while True: retrieve sockets that have events and have registered in the epoll object
    7.1. if the socket is the server_socket
    7.1.1 accept
    7.1.2 register the new client_socket to the epoll object
    7.1.3 since we only register the file description in the epoll object, and we cannot get the socket object from the poll() method, we need to create a dictionary that records the <fd,client_socket> (and <fd,client_addr> )
    7.2 else if the event of the socket is EPOLLIN
    7.2.3 recv
    7.2.3 if len(received_data)==0:close the socket & unregister the fd from the epoll
        else: get the data, print the data and send back 'hello' 
'''
import socket
import select

with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
    
    s.setblocking(False)

    my_addr=('127.0.0.1',2333)
    s.bind(my_addr)

    s.listen(5)

    epoll=select.epoll()

    epoll.register(s.fileno(),select.EPOLLIN|select.EPOLLET)

    fd_to_socket={}
    fd_to_addr={}

    while True:

        epoll_list=epoll.poll()
        
        for fd,event in epoll_list:
            
            if fd == s.fileno():
                client_socket,client_addr=s.accept()
            
                epoll.register(client_socket.fileno(),select.EPOLLIN|select.EPOLLET)

                fd_to_socket[client_socket.fileno()]=client_socket
                fd_to_addr[client_socket.fileno()]=client_addr

            elif event == select.EPOLLIN:
                
                client_socket=fd_to_socket[fd]
                recv_data=client_socket.recv(1024)

                if len(recv_data) == 0:
                    client_socket.close()
                    epoll.unregister(fd)
                else:
                    print("%s:%s"%(str(fd_to_addr[fd]),recv_data.decode('utf-8')))
                    client_socket.send('Thank you'.encode('utf-8'))

