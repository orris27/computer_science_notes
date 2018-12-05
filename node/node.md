## 1. npm和node安装
+ npm和node版本更新很快,不推荐yum安装
```
mkdir ~/tools
cd ~/tools
wget http://nodejs.org/dist/latest/node-v11.3.0-linux-x64.tar.gz
tar zxf node-v11.3.0-linux-x64.tar.gz 
mv node-v11.3.0-linux-x64/ /usr/local/
ln -s /usr/local/node-v11.3.0-linux-x64/ /usr/local/node
echo 'PATH=/usr/local/node/bin:$PATH' >> /etc/profile
source /etc/profile
```
