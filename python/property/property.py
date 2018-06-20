'''
    Learn property
'''
class Property1(object):
    def __init__(self,num):
       self.__num=num

    def get_num(self):
        return self.__num

    def set_num(self,new_num):
        self.__num=new_num
    
    num=property(get_num,set_num) # 1st param: getter | 2nd param: setter

class Property2(object):
    def __init__(self,num):
       self.__num=num

    @property # the name must be num
    def num(self):
        return self.__num

    @num.setter # the method name is also num
    def num(self,new_num):
        self.__num=new_num

p1=Property1(5)

# design get and set function
print(p1.get_num())
p1.set_num(100)
print(p1.get_num())

print('-'*30)

# property 1
print(p1.num)
p1.num=200
print(p1.num)

print('-'*30)

# property 2
p2=Property2(500)
print(p2.num)
p2.num=600
print(p2.num)
p2.num=600

