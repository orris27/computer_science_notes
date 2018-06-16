'''
    Modifying the same global variable at the same time using threads will cause problem
'''
import threading
import time

money=0
money_lock=False

def typing():
    global money
    global money_lock
    while True:
        if money_lock == False:
            money_lock=True
            for i in range(100000):
                money+=1
            money_lock=False
            break
    print('money in typing is %d'%(money))

def watering():
    global money
    global money_lock
    while True:
        if money_lock == False:
            money_lock=True
            for i in range(300000):
                money+=1
            money_lock=False
            break
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


