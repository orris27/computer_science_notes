'''
    Synchronize
    Passing ball game
    1. one is not locked and the rest are locked
    2. 1st says that I lock myself, and execute some codes, then unlock 2nd
    3. 2nd says that I lock myself, and execute some codes, then unlock 3rd 
    3. 3rd says that I lock myself, and execute some codes, then unlock 1st
'''

import threading
import time

lock1=threading.Lock()
lock2=threading.Lock()
lock2.acquire()
lock3=threading.Lock()
lock3.acquire()

def first():
    while True:
        if lock1.acquire():
            print('---1---')
            time.sleep(0.5)
            lock2.release()

    
def second():
    while True:
        if lock2.acquire():
            print('---2---')
            time.sleep(0.5)
            lock3.release()
            
def third():
    while True:
        if lock3.acquire():
            print('---3---')
            time.sleep(0.5)
            lock1.release()

t1=threading.Thread(target=first)
t1.start()
t2=threading.Thread(target=second)
t2.start()
t3=threading.Thread(target=third)
t3.start()
