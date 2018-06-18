'''
    Do a() for some time and do b() for some time
    1. create a function called 'a'
    1.1 while True:print 'A' & sleep for one second
    1.2 yield
    2. create a function called 'b' (similarity to a)
    3. while True: do a, then do b
'''
import time

def a():
    while True:
        print('---A---')
        time.sleep(1)
        yield

def b():
    while True:
        print('---B---')
        time.sleep(1)
        yield

if __name__ == '__main__':

    a_handler=a()
    b_handler=b()
    while True:
        next(a_handler)
        next(b_handler)
