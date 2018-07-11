## Linux安装常见问题
### 1. error while loading shared libraries: libpcre.so.1: cannot open shared object file: No such file or directory
解决办法:  
指定库或者在安装xx的时候指定目录  
下面给出的是指定库的方法
```
find / -name libpcre.so* # 先尝试找到这个文件
# /usr/local/lib/libpcre.so.1 # 如果找到这个文件了
echo '/usr/local/lib' >> /etc/ld.so.conf # 将这个路径放到/etc/ld.so.conf里面
ldconfig # 生效该配置文件
```
