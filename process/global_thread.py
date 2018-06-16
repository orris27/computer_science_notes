import threading
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


t1 = threading.Thread(target=add)
t1.start()

time.sleep(1)

t2 = threading.Thread(target=get)
t2.start()
