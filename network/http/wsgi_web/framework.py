import server

class Application(object):
    '''
        A class that implements a web framework.
    '''
    def __init__(self,urls,static_dir="static"):
        '''
            :urls:List of  key value pairs that records <path,function_name>, such as "[('/ctime',ctime),]"
            :static_dir: A directory that stores static files
        '''
        self.urls=urls
        self.static_dir=static_dir

    
    def __call__(self,env,start_response):
        '''    
            The server can treat the Application object as a application function (WSGI) 
            :env: a dictionary that records the path info in the "PATH_INFO"
            1. Get the path
            2. Find the corresponding function according to the path
            3. If found:
                3.1 Call this function and return the response body
            3. If not found:
                3.1 Construct the response header and body by ourselves
                3.2 return the response body
        '''    
        path=env.get("PATH_INFO","/") # default to "/"

        # static file
        import os
        if path.startswith('/static/'):
            try:
                f=open(os.path.join(self.static_dir,path[8:]))
            except:
                start_response('404 Not Found',[])
                return "Not Found"
            else:
                start_response('200 OK',[('Content-Type','text/html')])
                response_body=f.read()
                f.close()
                return response_body

        # dynamic file
        for url,handler in self.urls:
            if url==path:
                return handler(env,start_response)
        
        # Not Found:
        start_response("404 Not Found",[])
        response_body="Not Found"
        return response_body
        
def ctime(env,start_response):
    '''
        :!!:The function that corresponds to the url should follow WSGI
    '''
    import time
    start_response('200 OK',[('Content-Type','text/html')])
    return time.ctime()
        
def welcome(env,start_response):
    start_response('200 OK',[('Content-Type','text/html')])
    return '<html><h1>Welcome!</h1></html>'


urls=[
    ("/",welcome),
    ("/ctime",ctime),
]

app=Application(urls)


def main():
    '''
        :How to add new url?: 1. Add into urls 2. Define a WSGI function
    '''
    urls=[
        ("/",welcome),
        ("/ctime",ctime),
    ]

    app=Application(urls)
    http_server=server.HTTPServer(app)
    http_server.bind(('127.0.0.1',2333))
    http_server.run(100)

if __name__ == '__main__':
    main()
