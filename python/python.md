    
## 1. 安装Python3
1. 安装python3
```
##########################################################################################
# 确认环境
##########################################################################################
cat /etc/redhat-release 
#-------------------------------------------------------------------------------------------
# CentOS Linux release 7.3.1611 (Core) 
#-------------------------------------------------------------------------------------------
uname -r
#-------------------------------------------------------------------------------------------
# 3.10.0-514.26.2.el7.x86_64
#-------------------------------------------------------------------------------------------
uname -m
#-------------------------------------------------------------------------------------------
# x86_64
#-------------------------------------------------------------------------------------------


yum install zlib-devel bzip2-devel openssl-devel ncurses-devel sqlite-devel readline-devel tk-devel gcc make -y
yum -y install epel-release
yum install python-pip -y
mkdir tools
cd tools/
wget https://www.python.org/ftp/python/3.6.4/Python-3.6.4.tar.xz

xz -d Python-3.6.4.tar.xz
tar -xf Python-3.6.4.tar
cd Python-3.6.4
./configure prefix=/usr/local/python3
make && make install
mv /usr/bin/python /usr/bin/python.bak
ln -s /usr/local/python3/ /usr/local/python
ln -s /usr/local/python/bin/python3.6 /usr/bin/python
python -V

mv /usr/bin/pip /usr/bin/pip.bak
ln -s /usr/local/python/bin/pip3 /usr/bin/pip
pip -V


vim /usr/bin/yum 
#########################################################################
#! /usr/bin/python2
#########################################################################


vim /usr/libexec/urlgrabber-ext-down 
#########################################################################
#! /usr/bin/python2
#########################################################################
```

2. 安装tensorflow,ipython
```
pip install -U tensorflow
pip install ipython
echo 'PATH=/usr/local/python/bin/:$PATH'>>/etc/profile
source /etc/profile

```
