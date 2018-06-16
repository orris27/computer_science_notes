'''
    Passing parameters to connect two functions
    1. create two threads
    2. define a function(A) that calls two functions(1st:add ' allow';2nd:add ' pass')
    3. point the threads to the functionA
    4. start the threads 
'''
import threading
#import time

local_res=threading.local()

def stamp(name):
    local_res.name=name 
    check_family()
    #time.sleep(1)
    check_school()
    print("I am %s, and my result is %s"%(threading.current_thread().name,local_res.name))

def check_family():
    local_res.name+='-allow'
    return local_res.name

def check_school():
    local_res.name+='-pass'
    return local_res.name
    
    

t1=threading.Thread(target=stamp,args=['orris'],name="Thread-Orris")
t1.start()
t2=threading.Thread(target=stamp,args=['violet'],name="Thread-Violet")
t2.start()
