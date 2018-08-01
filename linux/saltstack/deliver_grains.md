## 1. 在master端写grains,然后分发给minion
1. 在file_root的环境下写对应grains的python脚本
+ 自定义的要放在`_grains`目录下面
```
cd /srv/salt/base
mkdir _grains
cd _grains

cat > my_grains.py <<EOF
#!/usr/bin/env python
def my_grains():
    '''
    My Custom Grains
    '''
    grains={'my_key1':'my_value1','my_key2':'my_value2'}
    return grains
EOF
# cat >> my_grains.py <<EOF
# my_grains()
# EOF
# python my_grains.py # 可以在py文件里调用这个函数,然后用python检验下是否正确
```
2. 把master端在`_grains`定义的grains同步到minion端
+ 默认放在minion的`/var/cache/salt/minion/extmods/grains`里面
```
salt '*' saltutil.sync_grains
```
3. 检测
```
salt '*' grains.item my_key1
salt '*' grains.item my_key2
```
