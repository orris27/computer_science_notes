import threading
import ctypes

def deadloop():
    while True:
        pass

lib=ctypes.cdll.LoadLibrary("./deadloop.so")

t=threading.Thread(target=lib.deadloop)
t.start()


deadloop()
