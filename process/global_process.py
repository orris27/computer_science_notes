import multiprocessing 
import time

g_var=100

def add():
    global g_var
    for i in range(5):
        g_var+=1

    print('the value in add is %d'%(g_var))

def get():
    global g_var

    print('the value in get is %d'%(g_var))


p1 = multiprocessing.Process(target=add)
p1.start()

time.sleep(1)

p2 = multiprocessing.Process(target=get)
p2.start()
