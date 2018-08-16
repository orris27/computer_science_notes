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
    1. 可以指定文件/通配符/`git add .`
    2. 功能
        1. 合并发生冲突的时候把冲突的文件标记为解决状态
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
11. `branch`:List, create, or delete branches
    1. `-d`:删除分支
12. `checkout`:Switch branches or restore working tree files
13. `merge`:合并分支
14. `stash`:本来工作在分支2上,还没处理完后就要移动到急需处理的分支3上,而git会不允许我们直接切换分支.stash就是用来解决这个问题的.
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

3. git分支测试
```
mkdir ~/my-branch
cd ~/my-branch

git init

echo 'readme' > README
echo '<h1>Hello</h1>' > index.html
echo 'MIT' > LICENSE

git add .
git commit -m "initial files"

git branch testing # 直接创建会在当前commit对象上建立指针

git branch # 列出所有分支 * master
git status # On branch master(上面2个都表示在master分支上工作)

git checkout testing # 切换到testing分支上

git branch
git status

echo "testing-add" >> index.html
git commit -am "add testing info to the index.html"

git checkout master

cat index.html # 会发现没有testing-add,说明回到了之前的状态

echo "master-add" >> index.html

git commit -am "add master info to the index.html"

```
4. git分支合并测试
    1. 已经在master默认分支上写了不少代码了
    2. 有个瓜皮Alice跟我说我有个代码写错了,好吧我只能创建个分支iss53来处理这个bug
    3. 另一个瓜皮Bob也跟我说我还有个地方代码写错了,行吧,我就回到还正确的master分支上再创建个新的分支hotfix来处理这个新bug
    4. 我处理完hotfix分支要解决的bug后,就合并到master上,这样Bob说的bug我就修复完了
    5. Alice说的bug我可能还没修复完,所以我继续修复Alice说的bug(回到iss53的分支修改).修改完后就合并到正确的master分支上.
    6. 大功告成!!
```
mkdir ~/my-merge
cd ~/my-merge

git init

echo '<h1>Hello</h1>' > index.html

git add index.html
git commit -m "add index.html"

echo '<h2>saber</h2>' > index.html
git commit -am "enable index.html to greet saber"

echo '<h2>mirai</h2>' >> index.html
git commit -am "enable index.html to greet mirai"


#################################################################
# 发现错误,说不能和saber打招呼5555,所以要创建个分支处理这个问题...
#################################################################
git checkout -b iss53 # <=> 先创建iss53的分支并且切换到iss53分支上

echo '<h2>no saber...</h2>' >> index.html
git commit -am "enable index.html to not greet saber"


#################################################################
# 发现另一个错误,说不能和mirai打招呼5555,所以要创建个分支处理这个问题...
#################################################################
git checkout master
git checkout -b hotfix

echo '<h2>no mirai...</h2>' >> index.html
git commit -am "enable index.html to not greet mirai"

#分支情况如下所示
#-------------------------------------------------------------
#                      hotfix
#                        |
#               master  c4
#                 |    /
# c0 --- c1 ---  c2
#                    \
#                      c3
#                       |
#                       iss53
#-------------------------------------------------------------

#################################################################
#合并master分支和hotfix分支,然后删除hotfix分支
#################################################################
git checkout master
git merge hotfix # Fast-foward:由于master在hotfix的后面,所以就是前进master分支
git branch -d hotfix # 删除hotfix分支
#分支情况如下所示
#-------------------------------------------------------------
#                      master
#                       |
#                      hotfix(will be deleted)
#                        |
#                       c4
#                      /
# c0 --- c1 ---  c2
#                    \
#                      c3
#                       |
#                       iss53
#-------------------------------------------------------------

#################################################################
# 继续回到iss53工作(因为iss53的工作不一定在说不能给mirai打招呼时干完),让index.html可以和saber,mirai打招呼
#################################################################
git checkout iss53
echo '<h2>saber & mirai</h2>' >> index.html
git commit -am "enable index.html to greet saber & mirai"


#分支情况如下所示
#-------------------------------------------------------------
#                      master
#                       |
#                       c4
#                      /
# c0 --- c1 ---  c2
#                    \
#                      c3 ---  c5
#                              |
#                            iss53
#-------------------------------------------------------------



#################################################################
# 合并master和iss53,并处理相关差异
#################################################################
git checkout master
git merge iss53 # 如果直接merge的话会可能会不允许,比如这里我的master有'1\n2\n3'这样的内容,而如果iss53的内容是'1\n5'的话就不能直接合并.git给出的策略是在index.html文件里写入差异,然后我们人工修改index.html这个文件,然后提交到master的版本库里,这样就能merge了
#-------------------------------------------------------------
Auto-merging index.html
CONFLICT (content): Merge conflict in index.html
Automatic merge failed; fix conflicts and then commit the result.
#-------------------------------------------------------------

git status
#-------------------------------------------------------------
On branch master
You have unmerged paths.
  (fix conflicts and run "git commit")
  (use "git merge --abort" to abort the merge)

Unmerged paths:
  (use "git add <file>..." to mark resolution)

	both modified:   index.html

no changes added to commit (use "git add" and/or "git commit -a")
#-------------------------------------------------------------


cat index.html  # 人工修改这个文件
#-------------------------------------------------------------
<h2>saber</h2>
<h2>mirai</h2>
<<<<<<< HEAD
<h2>no mirai...</h2>
=======
<h2>no saber...</h2>
<h2>saber & mirai</h2>
>>>>>>> iss53
#-------------------------------------------------------------

vim index.html
###########################################################################
<h2>saber</h2>
<h2>mirai</h2>
<h2>no mirai...</h2>
<h2>no saber...</h2>
<h2>saber & mirai</h2>
###########################################################################


git commit -am "edit and merge master and iss53"

git status
#-------------------------------------------------------------
# On branch master
# nothing to commit, working tree clean
#-------------------------------------------------------------

git branch -d iss53
# 由于master和iss53在不同折线上,所以合并的时候,git会根据自己的算法获得比较好的共同祖先(这里是c2),然后三方面进行合并
#分支情况如下所示
#-------------------------------------------------------------
#                      master
#                       |
#                       "c4"
#                      /
# c0 --- c1 ---  "c2"
#                    \
#                      c3 ---  "c5"
#                              |
#                            iss53
#-------------------------------------------------------------
```
5. git stash测试
    1. 先设置环境
        1. master分支上创建个feature3分支并工作一会儿
        2. master分支上创建个working的工作分支并工作一会儿
    2. 听说feature3分支上有急事要处理,所以我就暂时保存working分支上的工作
    3. 切换到feature3分支处理急事
    4. 切换回工作分支,然后恢复到原来暂存的状态
```
mkdir ~/my-stash
cd ~/my-stash

git init

echo '<h1>Hello</h1>' > index.html

git add index.html
git commit -m "add index.html"

git checkout -b feature3
echo "feature3" >> index.html
git commit -am "add feature3 in index.html"

git checkout master
git checkout -b working

echo "working" >> index.html

#########################################################
# 我们工作一半的时候,有人跟我们说代码有问题
#########################################################
git status
git checkout feature3 # 由于工作区的内容没提交,所以就不允许切换,除非暂时储藏起来
#-------------------------------------------------------------------
# error: Your local changes to the following files would be overwritten by checkout:
# 	index.html
# Please commit your changes or stash them before you switch branches.
# Aborting
#-------------------------------------------------------------------

git stash # 这样就保存了当前工作区的状态
git stash list # 可以查看状态列表

git checkout feature3

echo "new-feature3" >> index.html
git commit -am "add new-feature3 in index.html" 

git checkout working #这时候工作区是干净的
cat index.html

git stash list
git stash apply stash@{0} # 回到工作区之前的状态
cat index.html
```
