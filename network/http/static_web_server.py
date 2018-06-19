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
import os

STATIC_DIR='static'

def handle_client(client_socket,client_addr):
    '''
        A function that handles the client's request
        1. recv 
        2. deal with the message
            2.1 get the required filename
            2.2 open the file
            2.3 write the file to the response
        3. send back
    '''
    recv_data=client_socket.recv(1024)
    request_start_line=recv_data.decode('utf-8').split('\r\n')[0]
    import re
    print(request_start_line)
    filename=re.match('\w+\ (/[^\ ]*)\ ',request_start_line).group(1)

    if filename == '/':
        filename += 'index.html'

    # os.path.join('static','/index.html') returns '/index.html'; so we need to strip '/' in the second parameter
    filename=os.path.join(STATIC_DIR,filename[1:])

    
    # construct a respone
    try:
        f=open(filename,'rb')
    except:
        response_start_line=b'HTTP/1.1 404 Not Found\r\n'
        response_headers=b'\r\n' # do not forget \r\n!!! (if empty,then there is a problem)
        response_body=b"404 Not Found"
    else:
        response_start_line=b'HTTP/1.1 200 OK\r\n'
        response_headers=b'Content-Type: text/html\r\n'
        response_body=f.read()
        f.close()

    response=response_start_line+response_headers+b'\r\n'+response_body

    client_socket.send(response)

    client_socket.close()

with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)

    s.bind(('127.0.0.1',2333))

    s.listen(100)

    while True:

        client_socket,client_addr=s.accept()
        
        print("%s:%s connected."%(str(client_addr[0]),str(client_addr[1])))

        p = multiprocessing.Process(target=handle_client,args=[client_socket,client_addr])
        
        p.start()

        client_socket.close()
