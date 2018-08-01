## 1. 在master端写grains,然后分发给minion
1. 在file_root的环境下写对应grains的python脚本
+ 自定义的要放在`_grains`目录下面
```
cd /srv/salt/base
mkdir _grains
cd _grains

vim my_grains.py
#############
#!/usr/bin/env python
def my_grains():
    '''
    My Custom Grains
    '''
    grains={'my_key1':'my_value1','my_key1':'my_value1'}
    return grains
#############
```
