## dApp Installation
### go
```
rpm -ivh https://mirrors.aliyun.com/epel/epel-release-latest-7.noarch.rpm
yum install -y wget

mkdir ~/tools
cd ~/tools


wget https://dl.google.com/go/go1.7.3.linux-amd64.tar.gz
sudo tar -C /usr/local -xzf go1.7.3.linux-amd64.tar.gz 
echo 'export PATH=$PATH:/usr/local/go/bin' >> /etc/profile
echo 'export GOPATH=/usr/local/go' >> /etc/profile
echo 'export GOBIN=$GOPATH/bin' >> /etc/profile
echo 'export PATH=$PATH:$GOBIN' >> /etc/profile
source /etc/profile
go version



wget http://nodejs.org/dist/v6.9.1/node-v6.9.1-linux-x64.tar.gz
tar zxf node-v6.9.1-linux-x64.tar.gz 
mv node-v6.9.1-linux-x64 /usr/local/
ln -s /usr/local/node-v6.9.1-linux-x64/ /usr/local/node
echo 'PATH=/usr/local/node/bin:$PATH' >> /etc/profile
source /etc/profile
npm -v # 3.10.8
node -v # 6.9.1




```
