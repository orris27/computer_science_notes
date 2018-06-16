from multiprocessing import Process
from multiprocessing import Queue

q=Queue(5)
q2=Queue(5)

def get(total):
    for i in range(total):
        print('adding num %d'%(i+1,))
        q.put(i+1)

def times(num):
    while True:
        if not q.empty():
            val=q.get()
            print('times %d for the num %d'%(num,val))
            val=val*num
            q2.put(val)
        else:
            break
            
def save(filename):
    with open(filename,'w') as f:
        while True:
            if not q2.empty():
                val=q2.get()
                print('save num %d into the file %s'%(val,filename))
                f.write(str(val)+'\n')
            else:
                break

p1 = Process(target=get,args=[3,])
p1.start()
p1.join()

p2 = Process(target=times,args=[10])
p2.start()
p2.join()

p3 = Process(target=save,args=['1.txt'])
p3.start()
p3.join()
while not q.empty():
    print('q:',q.get())
while not q2.empty():
    print('q2:',q2.get())
