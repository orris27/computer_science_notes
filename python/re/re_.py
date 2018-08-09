import re

pattern=re.compile('(\d+)no(\d+)')
m=pattern.match('hello123no456a',5)

print(m)
print(m.group(0))
print(m.group(1))
print(m.group(2))

pattern=re.compile(r'\d+\\')
m=pattern.search('helo123\\  123')
print(m)
print(m.group())

pattern=re.compile(r'(\d+)(\w+)')
m=pattern.findall('whalkjd 123jdk 123jaads j21')
print(m)


pattern=re.compile(r'\d+')
m=pattern.finditer('asdasldj1askdjl2lkjlsad3')
for i in m:
    print(i.group())



pattern=re.compile(r'stupid|fuck',re.I)
m=pattern.sub('**',"You are stupid!Fuck!")
print(m)
