import threading

def deadloop():
    while True:
        pass

t=threading.Thread(target=deadloop)
t.start()

deadloop()
