## Nginx-1.14.0
### 安装
#### 1. 安装准备
##### 1-1. 安装pcre
pcre的编译安装比较麻烦,所以选择yum安装
```
sudo yum install pcre pcre-devel
```
##### 1-2. 安装openssl
```
sudo yum install openssl openssl-devel
```

#### 2. 常规步骤
```
tar -zxvf nginx-1.14.0.tar.gz 
cd nginx-1.14.0/
sudo  useradd nginx -s /sbin/nologin -M
sudo ./configure --user=nginx --group=nginx --prefix=/application/nginx-1.14.0 --with-http_stub_status_module --with-http_ssl_module
sudo make && sudo make install
sudo ln -s /application/nginx-1.14.0/ /application/nginx
```

### 使用
```
sudo /application/nginx/sbin/nginx
```
