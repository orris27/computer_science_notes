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

## 2. git
### 2-1. options
1. `init`:初始化目录,使当前文件夹成为仓库
2. `add`:添加文件到index(git中其实是暂存区,缓冲区)中,让git去跟踪该文件
    1. 可以指定文件/通配符
    2. 功能
        1. 合并的时候把冲突的文件标记为解决状态
        2. 跟踪新文件
        3. 将已跟踪的文件从工作区更新到暂存区
3. `help`:查看帮助,如`git help config`
4. `status`:查看缓冲区存储了哪些修改
5. `remote`:
6. `commit`:将跟踪的文件提交到版本库(仓库)里
    1. `-a`:将工作区中的文件放到暂存区中,再放到版本库里(`git add`+`git commit`的组合)
7. `push`:
8. `pull`:
9. `config`:Get and set repository or global options
10. `log`:Show commit logs
    + git是分布式版本控制库,里面的id是sha-1算法计算得到的
### 2-2. 使用
1. 基本操作
    1. 配置用户信息
    2. 创建一个工作区,并初始化为仓库
    3. 在工作区里工作...
    4. 添加文件到暂存区里
    5. 提交暂存区里的数据
    6. 查看暂存区里的数据
```
git config --global user.name "orris"
git config --global user.email xxx@163.com

git help config

mkdir ~/mysite
cd ~/mysite
git init

echo '<h1>Hello</h1>'> index.html

git add index.html # git add *.html

git commit -m "add hello world html"

git log
#--------------------------------------------------------------------
# commit 53b96552c1a6b6ae1624beda6f29d6a65e316357 # 使用sha1算法方便管理
# Author: orris <993790240@qq.com>
# Date:   Thu Aug 16 18:47:31 2018 +0800
#
#     add index.html
#--------------------------------------------------------------------
```


2. git-status测试
```
mkdir ~/mystatus
cd ~/mystatus
git init 

echo '<h1>Hello</h1>'> index.html
git status

git add index.html
git status

git commit 
git status
```

