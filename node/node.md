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
## 2. npm安装
安装
```
npm install
```
忽略phantomjs这些权限错误
```
npm install --unsafe-perm
```
重新安装
```
rm -rf ./node_modules/
```
