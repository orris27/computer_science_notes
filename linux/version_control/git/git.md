## 1. 安装
通过yum安装的git版本太低,所以只能选择源码安装
1. 安装依赖包
2. 下载git源码
3. 使用git的编译方式编译并安装git
4. 添加git命令到环境变量
5. 验证
```
# yum info git
yum install curl-devel expat-devel gettext-devel openssl-devel zlib-devel -y
yum install gcc perl-ExtUtils-MakeMaker autoconf asciidoc xmlto docbook2X -y
ln -s /usr/bin/db2x_docbook2texi /usr/bin/docbook2x-texi


yum install -y wget

# yum remove git
mkdir ~/tools
cd ~/tools

wget https://mirrors.edge.kernel.org/pub/software/scm/git/git-2.9.5.tar.gz
tar -zxf git-2.9.5.tar.gz
cd git-2.9.5

less INSTALL 
make configure
./configure --help
mkdir /application
./configure --prefix=/application/git-2.9.5
make all doc info
make install install-doc install-html install-info

ln -s /application/git-2.9.5/ /application/git
echo 'export PATH=/application/git/bin:$PATH' >> /etc/profile
source /etc/profile

git --version
```

## 2. 使用
1. 配置用户信息



