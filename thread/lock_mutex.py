'''
    Mutex application.
    1. buy a lock
    2. lock {code blocks} unlock
'''
import threading
import time

money=0
#money_lock=False

mutex=threading.Lock() # buy a lock

def typing():
    global money
#    global money_lock
#    while True:
#        if money_lock == False:
#            money_lock=True
#     mutex.acquire()
    for i in range(100000):
        mutex.acquire()
        money+=1
        mutex.release()
#     mutex.release()
#            money_lock=False
#            break
    print('money in typing is %d'%(money))

def watering():
    global money
#    global money_lock
#    while True:
#        if money_lock == False:
#            money_lock=True
#     mutex.acquire() # We should lock the minimal block that needs to be locked
    for i in range(300000):
        mutex.acquire()
        money+=1
        mutex.release()
#     mutex.release()
#            money_lock=False
#            break
    print('money in watering is %d'%(money))

def check():
    global money
    print('money in check is %d'%(money))


t1 = threading.Thread(target=typing)
t1.start()

#time.sleep(1)

t2 = threading.Thread(target=watering)
t2.start()

#time.sleep(1)
time.sleep(4)

t3 = threading.Thread(target=check)
t3.start()
