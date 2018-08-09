## 1. 语法
1. 匹配规则 
    1. 匹配的字符串 <=> egrep里的字符串
    2. 是否忽略大小写
        + `re.I`
2. 匹配方式
    1. 贪婪模式 vs 非贪婪模式
    2. 匹配所有 vs 匹配第一个
    
3. 字符串

4. 处理
    1. 是否替换
        + `sub`
### 1-2. 实例
1. 基本re方法
> https://github.com/orris27/orris/blob/master/python/re/re_.py
```
import re

pattern=re.compile('(\d+)no(\d+)')
m=pattern.match('hello123no456a',5)

print(m)
#<_sre.SRE_Match object; span=(5, 13), match='123no456'>
print(m.group(0))
#123no456
print(m.group(1))
#123
print(m.group(2))
#456

pattern=re.compile(r'\d+\\')
m=pattern.search('helo123\\  123')
print(m)
#<_sre.SRE_Match object; span=(4, 8), match='123\\'>
print(m.group())
#123\

pattern=re.compile(r'(\d+)(\w+)')
m=pattern.findall('whalkjd 123jdk 123jaads j21')
print(m)
#[('123', 'jdk'), ('123', 'jaads'), ('2', '1')]


pattern=re.compile(r'\d+')
m=pattern.finditer('asdasldj1askdjl2lkjlsad3')
for i in m:
    print(i.group())
#1
#2
#3

pattern=re.compile(r'stupid|fuck',re.I)
m=pattern.sub('**',"You are stupid!Fuck!")
print(m)
#You are **!**!
```
