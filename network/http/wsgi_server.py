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
    def __init__(self,static_dir,wsgi_dir):
        '''
            static_dir: the path to the static files
        '''
        self.server_socket=socket.socket(socket.AF_INET,socket.SOCK_STREAM)

        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)

        self.static_dir=static_dir

        self.wsgi_dir=wsgi_dir
        
        sys.path.insert(1,self.wsgi_dir)



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
        
        self.response_header=self.response_header.encode('utf-8')

    def handle_client(self,client_socket,client_addr):
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

        env={}

        if filename.endswith('.py'):

            try:

                f=__import__(filename[1:-3])
            except:
                response_body=""
            else:
                # After application, the response header and the response body has been created
                response_body=f.application(env,self.start_response)

            # Splicing the response header and the response body
            response=self.response_header+'\r\n'.encode('utf-8')+response_body.encode("utf-8")

        else:
            if filename == '/':
                filename += 'index.html'

            filename=os.path.join(self.static_dir,filename[1:])

            # construct a respone
            try:
                f=open(filename,'rb')
            except:
                response_start_line=b'HTTP/1.1 404 Not Found\r\n'
                response_headers=b'\r\n' # do not forget \r\n!!!
                response_body=b"404 Not Found"
            else:
                response_start_line=b'HTTP/1.1 200 OK\r\n'
                response_headers=b'Content-Type: text/html\r\n'
                response_body=f.read()
                f.close()

            response=response_start_line+response_headers+b'\r\n'+response_body

        client_socket.send(response)

        client_socket.close()
        
def main():

    STATIC_DIR='static'
    
    WSGI_DIR='wsgipython'

    server=HTTPServer(STATIC_DIR,WSGI_DIR)

    server.bind(('127.0.0.1',2333))
    
    server.run(100)



if __name__ == "__main__":
    main()
