## 0. 优化准备
```
sudo yum install autoconf -y
```

## 1. PHP引擎加速与缓存优化模块
### <1> eaccelerator
好像不能用...
### <2> xcache
```
wget https://xcache.lighttpd.net/pub/Releases/3.2.0/xcache-3.2.0.tar.gz

```

## 常见问题
### 1.没有`./configure`文件
原因: `./configure`文件只有在`phpize`后才出现

### 2. 
phpize的时候出现下面信息
```
Cannot find autoconf. Please check your autoconf installation and the
$PHP_AUTOCONF environment variable. Then, rerun this script.
```
解决方法:
```
sudo yum install autoconf -y
```
