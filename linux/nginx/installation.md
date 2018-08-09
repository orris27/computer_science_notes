## Nginx-1.14.0
### 安装
1. 安装准备
    1. 安装pcre
        > pcre的编译安装比较麻烦,所以选择yum安装
    2. 安装openssl
2. 创建Nginx用户
    
3. 下载并解压


#### 编译安装
```
#######################################################################################
# 可以直接复制粘贴!!
#######################################################################################


sudo yum install pcre pcre-devel -y
sudo yum install openssl openssl-devel -y
sudo yum groupinstall "Development Tools" -y
sudo yum install wget -y

mkdir ~/tools
cd ~/tools

wget https://nginx.org/download/nginx-1.14.0.tar.gz
tar -zxvf nginx-1.14.0.tar.gz 
cd nginx-1.14.0/

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 优化Nginx(修改响应头和错误代码的Nginx服务器)
# 见 https://github.com/orris27/orris/blob/master/linux/nginx/optimization.md 的第14条优化
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sudo  useradd nginx -s /sbin/nologin -M
sudo ./configure --user=nginx --group=nginx --prefix=/application/nginx-1.14.0 --with-http_stub_status_module --with-http_ssl_module
sudo make && sudo make install
sudo ln -s /application/nginx-1.14.0/ /application/nginx

sudo ln -s /application/nginx/sbin/nginx /usr/local/bin/
```

### 使用
#### 1. 启动
```
sudo /application/nginx/sbin/nginx
```
#### 2. 平滑重启
```
sudo /application/nginx/sbin/nginx -t
sudo /application/nginx/sbin/nginx -s reload
```
### 问题
#### 1. nginx: [error] invalid PID number "" in "/application/nginx-1.14.0/logs/nginx.pid"
没有启动Nginx,使用`sudo nginx`就行了

