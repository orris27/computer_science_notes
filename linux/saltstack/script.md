## 1. 编写模块的脚本
1. 写模块的程序
+ salt自己的模块写在`/usr/lib/python2.6/site-packages/salt/modules`下
+ 可以参考`service.py`
+ py文件是什么名字,模块就是什么名字
```
cd /srv/salt/base
mkdir _modules
cd _modules
cat > my_disk.py <<EOF
def list():
    cmd = 'df -h'
    ret = __salt__['cmd.run'](cmd)
    return ret
EOF
```
2. 分发模块到minion中
```
salt '*' saltutil.sync_modules
```
3. 执行
```
salt  '*' my_disk.list
```
