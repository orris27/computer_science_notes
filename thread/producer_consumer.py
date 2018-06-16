'''
    Producer and Consumer
    1. we have some food at first
    2. create two kinds of threads, producer and consumer
    3. for producer: if food is not enough, then produce
    4. for consumer: if food is enough, then eat
'''
import threading
from queue import Queue



class Producer(threading.Thread):
    def run(self):
        global q
        count=0
        while True:
            if q.qsize() < 100:
                for i in range(30):
                    count+=1
                    q.put('Produced Food'+str(count))
                    print('%s produces food %d'%(self.name,count))

class Consumer(threading.Thread):
    def run(self):
        global q
        while True:
            if q.qsize() > 50:
                for i in range(5):
                    name=q.get()
                    name=name.replace('Produced Food','').replace('Initial Food','')             
                    print('%s eat food %s'%(self.name,name))

if __name__ == '__main__':
    q=Queue()

    for i in range(500):
        q.put('Initial Food'+str(i))
    
    for i  in range(2):
        p = Producer()
        p.start()

    for i in range(5):
        c = Consumer()
        c.start()



