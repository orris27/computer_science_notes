'''
    Passing parameters to connect two functions
    1. create two threads
    2. define a function(A) that calls two functions(1st:add ' allow';2nd:add ' pass')
    3. point the threads to the functionA
    4. start the threads 
'''
import threading

def stamp(name):
    
    res=name
    res=check_family(res)
    res=check_school(res)
    print(res)

def check_family(s):
    s+='-allow'
    return s

def check_school(s):
    s+='-pass'
    return s
    
    

t1=threading.Thread(target=stamp,args=['orris'])
t1.start()
t2=threading.Thread(target=stamp,args=['violet'])
t2.start()
