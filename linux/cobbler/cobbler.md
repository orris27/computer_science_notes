## 1. 安装
1. yum安装
```
yum install cobbler cobbler-web dhcp tftp-server pykickstart httpd -y
```


2. 重启Apache
```
systemctl restart httpd
```

3. 启动Cobbler
```
systemctl start cobblerd
```
4. 检测Cobbler
```
cobblerd check
```
