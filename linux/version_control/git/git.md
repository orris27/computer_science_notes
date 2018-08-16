## 1. 安装
通过yum安装的git版本太低,所以只能选择源码安装

```
# yum info git
yum install curl-devel expat-devel gettext-devel openssl-devel zlib-devel -y
yum install gcc perl-ExtUtils-MakeMaker autoconf asciidoc xmlto -y
yum install -y wget
# yum remove git
mkdir ~/tools
cd ~/tools

wget https://mirrors.edge.kernel.org/pub/software/scm/git/git-2.9.5.tar.gz
tar -zxf git-2.9.5.tar.gz
cd git-2.9.5
```

## 2. 使用
1. 配置用户信息



