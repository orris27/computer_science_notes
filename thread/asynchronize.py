'''
    1. create a thread to borrow a book
    2. I first do something else
    3. If he has got the book and returns to me, then I pause the current task and place the book in the shelf
    4. After that, I continue to do previous things
'''

import multiprocessing
import time
import os
import threading

def borrow(book):
    time.sleep(1)
    return book


def place(book):
    print('%s %s\tplacing %s starts'%(os.getpid(),threading.current_thread().name,book))
    time.sleep(5)
    print('%s %s\tplacing %s finish'%(os.getpid(),threading.current_thread().name,book))

pool=multiprocessing.Pool(3)
books=['orris','violet','young']

for i in range(3):
    pool.apply_async(func=borrow,args=[books[i]],callback=place)

for i in range(1000):
    time.sleep(0.5)
    print('%s %s\tI am doing some boring things...'%(os.getpid(),threading.current_thread().name))

print('completed')
