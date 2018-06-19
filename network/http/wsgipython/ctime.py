'''
    Return the current time
'''
import time

def application(env,start_response):
    start_response('200 OK',[("Content-Type","text/html")])
    return time.ctime()

def test():
    print(time.ctime())
