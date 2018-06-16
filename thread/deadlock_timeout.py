'''
    I do not apology, and you do not apology.
    For me: only if you apology, will I apology. (After 5s, I give up and apology)(there is a problem)
    For you: only if I apology, will you apology. (After 5s, you give up and apology)
'''
import threading
import time

my_lock=threading.Lock()
your_lock=threading.Lock()

def i():
    '''
        1. I am determined to not apology
        2. if you apology, I apology 
    '''
    if my_lock.acquire():
        print('I am angry, and I will not apology.')
    
        time.sleep(1)

        if your_lock.acquire(timeout=5):
            print('I apology.')
            my_lock.release()

            your_lock.release()

def you():

    if your_lock.acquire():
        print('you are angry, and you will not apology.')
    
        time.sleep(1)

        if my_lock.acquire(timeout=5):
            print('I apology')
            your_lock.release()

            my_lock.release()

    
t1=threading.Thread(target=i)
t1.start()

t2=threading.Thread(target=you)
t2.start()
