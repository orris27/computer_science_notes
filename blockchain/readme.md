## DApp Installation
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


npm install -g ganache-cli@6.1.0


git clone https://github.com/Blockchain-book/Ethereum-Score-Hella.git
cd Ethereum-Score-Hella/
npm install

##################################################################
# 创建账户 (放在新的窗口中)
##################################################################
truffle develop //开启环境
# 或者(二选一)
ganache-cli


##################################################################
# 修改源代码 + 编译运行
##################################################################
vim app/javascripts/app.js # 默认请求127.0.0.1:8545的ganache-cli等,改成对应服务器ip和端口(truffle develop是9545,而ganache-cli是8545)
##################################################################
window.addEventListener('load', function () {
  // 设置web3连接 http://127.0.0.1:8545. 默认是请求
  //window.web3 = new Web3(new Web3.providers.HttpProvider('http://127.0.0.1:8545'))
  window.web3 = new Web3(new Web3.providers.HttpProvider('http://47.101.175.159:9545'))
  window.App.init()
})
##################################################################


vim node_modules/webpack-dev-server/bin/webpack-dev-server.js  # 让npm run dev运行在0.0.0.0,而不是127.0.0.1,这样局域网就可以访问该守护进程
##################################################################
host: {
  type: 'string',
  //default: 'localhost',
  default: '0.0.0.0',
  describe: 'The hostname/ip address the server will bind to',
  group: CONNECTION_GROUP
},
##################################################################


truffle compile //编译
truffle migrate --network truffle 

npm run dev

###################################################################
# 比如注册账号为"0x93e66d9baea28c17d9fc393b53e3fbdd76899dac",密码为"123456",这样就注册成功了,然后登录就可以自由飞翔了
###################################################################



```


## concepts
truffle 

ganache-cli 和 truffle develop都是创建10个账户,然后监听端口. 这时候truffle就可以compile + migrate到这些端口上,这样就可以启动服务了

truffle compile --compile-all 修改一个合约,但是项目有多个合约,就会编译所有合约

truffle migrate 部署智能合约. 将编译好后的智能合约部署到以太坊客户端(testRPC, ganache-cli, truffle develop等)中. 如果执行失败,可以用`--reset`


