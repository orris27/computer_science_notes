'''
    Create a TCP server using gevent
    1. patch !! very important
    2. create a socket(gevent's socket)
    3. bind
    4. listen
    5. while True:
    6.1 accept
    6.2 create a new coroutine to handle the client

'''
import gevent
from gevent import monkey

monkey.patch_all() # cannot say "gevent.monkey.patch_all()"

def handle_client(client_socket,client_addr):
    '''
        A function that handles the client's request
        1. while True:
        1.1 recv
        1.2 if len > 0: print and send back 'Thank you'
        1.3 else: close the socket
    '''

    while True:
        recv_data=client_socket.recv(1024)
        
        if len(recv_data) > 0:
            print("%s:%s"%(str(client_addr),recv_data.decode('utf-8')))
            client_socket.send('Thank you'.encode('utf-8'))

        else:
            client_socket.close()
            break

s=gevent.socket.socket() # no AF_INET & SOCK_STREAM
s.bind(('127.0.0.1',2333))
s.listen(100)

while True:

    client_socket,client_addr=s.accept()

    gevent.spawn(handle_client,client_socket,client_addr)





