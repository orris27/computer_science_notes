'''
    Do a() for some time and do b() for some time
    1. create a function called 'a'
    1.1 while True:print 'A' & sleep for one second
    1.2 swith to function b 
    2. create a function called 'b' (similarity to a)
    3. while True: do a
'''
import greenlet
import time

def a():
    while True:
        print('---A---')
        time.sleep(1)
        b_handler.switch()


def b():
    while True:
        print('---B---')
        time.sleep(1)
        a_handler.switch()


if __name__ == '__main__':

    a_handler=greenlet.greenlet(a)
    b_handler=greenlet.greenlet(b)

    a_handler.switch()
