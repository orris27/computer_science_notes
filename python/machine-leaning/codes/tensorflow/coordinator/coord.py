'''
    Intro: Create some threads and entrust a Coordinator to manage the threads
    Usage: python coord.py
'''
import tensorflow as tf
import numpy as np
import threading
import time

# define target of a thread
def work(coord, i):
    # while haven't receive stop info
    while not coord.should_stop():
        # generate a random number
        num = np.random.rand()
        # if number < 0.1:
        if num < 0.1:
            # print that I say stop
            print('%d asks to stop'%(i))
            # request stop
            coord.request_stop()
        # else:
        else:
            # print my own info
            print('%d'%(i))
            # sleep 1s
            time.sleep(1)


# create a coord
coord = tf.train.Coordinator()
# create a threading pool
threads = [threading.Thread(target=work,args=(coord,i)) for i in range(5)]
# start all threads
for thread in threads:
    thread.start()
# join all threads
coord.join(threads)
