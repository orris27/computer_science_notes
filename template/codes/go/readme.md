## installation
https://golang.org/doc/install
```
mkdir ~/tools
cd ~/tools

wget https://dl.google.com/go/go1.11.4.linux-amd64.tar.gz
sudo tar -C /usr/local -xzf go1.11.4.linux-amd64.tar.gz 
echo 'export PATH=$PATH:/usr/local/go/bin' >> /etc/profile
echo 'export GOPATH=/usr/local/go' >> /etc/profile
echo 'export GOBIN=$GOPATH/bin' >> /etc/profile
echo 'export PATH=$PATH:$GOBIN' >> /etc/profile

source /etc/profile

go version

```

