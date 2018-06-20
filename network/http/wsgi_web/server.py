'''
    A HTTP server that is packaged using class(the same as "static_web_server.py")
    This http server can retrieve html file in the './static' and run & print the result of xx.py in the './wsgipython'
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
import sys

class HTTPServer(object):
    '''
        A class that creates a http server
    '''
    def __init__(self,application):
        '''
            static_dir: the path to the static files
        '''
        self.application=application

        self.server_socket=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)

    def run(self,listen_num):
        '''
            listen_num: the maximum number of listeners
        '''

        self.server_socket.listen(listen_num)

        while True:

            client_socket,client_addr=self.server_socket.accept()
            
            print("%s:%s connected."%(str(client_addr[0]),str(client_addr[1])))

            p = multiprocessing.Process(target=self.handle_client,args=[client_socket,client_addr])
            
            p.start()

            client_socket.close()

    def bind(self,addr):
        '''
            addr: a tuple (ip,port)
        '''
        self.server_socket.bind(addr)


    def start_response(self,status,headers):
        '''
            Construct our response header completely
            :status:(eg. '200 OK')
            :headers:(eg. [('Content-Type','text/html')])
            :return:NoneType
            :modify self.response_header:(eg. 'HTTP/1.1 200 OK\r\nxxx: xxx\r\n\')
        '''
        self.response_header='HTTP/1.1 '+status+'\r\n'
        
        for header in headers:
            self.response_header+="%s: %s\r\n"%(header[0],header[1]) # ": " has a space
        
        self.response_header+="Author: orris\r\n"
        

    def handle_client(self,client_socket,client_addr):
        '''
            A function that handles the client's request
            1. recv 
            2. deal with the message
                2.1 get the required path, such as "/","/ctime"
                2.2 Construct "env" dict( add "PATH_INFO"...)
                2.3 Call application member to deal with my response header and get the body
                2.4 Construct response (splice the header and body)
            3. send back
        '''
        recv_data=client_socket.recv(1024)
        request_start_line=recv_data.decode('utf-8').split('\r\n')[0]
        import re
        print(request_start_line)
        path=re.match('\w+\ (/[^\ ]*)\ ',request_start_line).group(1)
        
        env={"PATH_INFO":path}
        
        response_body=self.application(env,self.start_response)
        response=self.response_header+'\r\n'+response_body
        
        client_socket.send(response.encode('utf-8'))

        client_socket.close()
        
