'''
    Create a coroutine program using gevent
    1. create a function 
    2. create a gevent spawn
    3. join(wait for them)
'''
import gevent

def f(n):
    for i in range(n):
        print(gevent.getcurrent(),i)
        # gevent requires its own function. (X time.sleep(1))
        gevent.sleep(1)

g1=gevent.spawn(f,5)
g2=gevent.spawn(f,5)
g3=gevent.spawn(f,5)
g1.join()
g2.join()
g3.join()
