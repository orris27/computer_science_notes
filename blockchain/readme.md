## dApp Installation
### go
```
mkdir ~/tools
cd ~/tools

wget https://dl.google.com/go/go1.10.2.linux-amd64.tar.gz
sudo tar -C /usr/local -xzf go1.10.2.linux-amd64.tar.gz 
echo 'export PATH=$PATH:/usr/local/go/bin' >> /etc/profile
echo 'export GOPATH=/usr/local/go' >> /etc/profile
echo 'export GOBIN=$GOPATH/bin' >> /etc/profile
echo 'export PATH=$PATH:$GOBIN' >> /etc/profile
source /etc/profile
go version



wget http://nodejs.org/dist/v10.3.0/node-v10.3.0-linux-x64.tar.gz
tar zxf node-v10.3.0-linux-x64.tar.gz 
mv node-v10.3.0-linux-x64 /usr/local/
ln -s /usr/local/node-v10.3.0-linux-x64/ /usr/local/node
echo 'PATH=/usr/local/node/bin:$PATH' >> /etc/profile
source /etc/profile
npm -v # 6.1.0
node -v # 10.3.0



npm install -g truffle@4.1.11
truffle -v


wget https://github.com/ethereum/go-ethereum/archive/v1.8.10.tar.gz
tar -xvf v1.8.10.tar.gz
cd go-ethereum-1.8.10/
make geth
cd build/bin/
./geth help
ln -s /root/tools/go-ethereum-1.8.10/build/bin/geth /usr/bin/geth
geth --version


```
