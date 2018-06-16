'''
    Passing parameters to connect two functions
    1. create two threads
    2. define a function(A) that calls two functions(1st:add ' allow';2nd:add ' pass')
    3. point the threads to the functionA
    4. start the threads 
'''
import threading

g_res={}

def stamp(name):
    
    g_res[threading.current_thread().name]=name
    check_family()
    check_school()
    print("I am %s, and my result is %s"%(threading.current_thread().name,g_res[threading.current_thread().name]))

def check_family():
    g_res[threading.current_thread().name]+='-allow'
    return g_res[threading.current_thread().name]

def check_school():
    g_res[threading.current_thread().name]+='-pass'
    return g_res[threading.current_thread().name]
    
    

t1=threading.Thread(target=stamp,args=['orris'])
t1.start()
t2=threading.Thread(target=stamp,args=['violet'])
t2.start()
