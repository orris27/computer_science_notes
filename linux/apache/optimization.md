### 1. 禁止对外提供目录
#### 解决方法
```
# httpd.conf
Options -Indexes +FollowSymLinks
```

### 2. 阻止Apache在-t/启动时的报错(非优化)
#### 解决方法
```
# httpd.conf
ServerName 127.0.0.1:80
```
### 3. 隐藏版本
#### 解决方法
1. 修改服务器版本:在`~/tools/httpd-2.4.33/include/ap_release.h`中修改宏定义
2. 修改运行后的操作系统:在`~/tools/httpd-2.4.33/os/unix/os.h`中修改宏定义
##### ap_release.h
```
#define AP_SERVER_BASEVENDOR "Apache Software Foundation"
#define AP_SERVER_BASEPROJECT "Apache HTTP Server"
#define AP_SERVER_BASEPRODUCT "Apache"

#define AP_SERVER_MAJORVERSION_NUMBER 2
#define AP_SERVER_MINORVERSION_NUMBER 4
#define AP_SERVER_PATCHLEVEL_NUMBER   33
#define AP_SERVER_DEVBUILD_BOOLEAN    0
```
##### os.h
```
#ifndef PLATFORM
#define PLATFORM "Unix"
#endif
```
### 4. 在响应头中隐藏版本号
1. 修改`apache/conf/extra/httpd-default.conf`下的参数
```
# modified into
ServerTokens Prod
ServerSignature Off
```
2. 在主配置文件中include该配置文件
```
Include conf/extra/httpd-default.conf
```

### 5. gzip压缩

