import threading
import ctypes

def deadloop():
    while True:
        pass

lib=ctypes.cdll.LoadLibrary("./deadloop.so") # open the library

t=threading.Thread(target=lib.deadloop) # use the "deadloop" function in the lib
t.start()


deadloop()
