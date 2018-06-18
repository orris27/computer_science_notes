'''
    Create a server that 1)can only serve at most one person at the same time 2)print the message from the client 3)send back a "thank you" message
    1. create a socket
    2. enable the socket to bind a same port
    3. bind
    4. listen
    5. select the sockets that can receive data
    6. travese the sockets
    7.  if it is server socket(it means that it can accept a new client): call accept()
    8.  else:(then it is a client socket and the client has sent a message to the server) call recv()
'''
import socket
import select

with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
    
    s.setblocking(False)

    my_addr=('127.0.0.1',2333)
    s.bind(my_addr)

    s.listen(5)
    # 不用设置server的socket为堵塞模式,client_socket也是一样

    inputs=[s]

    while True:

        # select会从第一个参数的列表中找到能接收数据的套接字,第二个...发送...,第三个...异常...;返回的列表就是那些套接字.否则就堵塞在这里
        acceptable,sendable,exceptional=select.select(inputs,[],[])

        for sock in acceptable:
            if sock == s:
                client_socket,client_addr=sock.accept()
                inputs.append(client_socket)
            
            else:
                received_data=sock.recv(1024)
                if len(received_data) == 0: # client closes his/her socket
                    sock.close()
                    inputs.remove(sock)
                else:
                    print("client:%s"%(received_data.decode('utf-8')))
                    sock.send('Thank you'.encode('utf-8'))
            
