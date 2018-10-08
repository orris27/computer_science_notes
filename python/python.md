    
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
mkdir ~/tools
cd ~/tools/
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


# vim /usr/bin/yum 
#########################################################################
# #! /usr/bin/python2
#########################################################################
sed -i "s#/usr/bin/python#/usr/bin/python2#g" /usr/bin/yum

# vim /usr/libexec/urlgrabber-ext-down 
#########################################################################
# #! /usr/bin/python2
#########################################################################
sed -i "s#/usr/bin/python#/usr/bin/python2#g" /usr/libexec/urlgrabber-ext-down 

pip install  tensorflow==1.8.0
pip install ipython
echo 'PATH=/usr/local/python/bin/:$PATH'>>/etc/profile
source /etc/profile

```

2. 安装tensorflow,ipython
```
pip install -U tensorflow
pip install ipython
echo 'PATH=/usr/local/python/bin/:$PATH'>>/etc/profile
source /etc/profile

```
3. 配置虚拟环境
    1. 普通pip
        1. `python2.7`: 最后我们在虚拟环境里的python
        2. `./venv`: 虚拟环境的名字
    ```
    sudo pip install -U virtualenv
    virtualenv --version
    #------------------------------------------------------------
    # 16.0.0
    #------------------------------------------------------------

    ###########################################################################
    # 创建虚拟环境的三种方式
    ###########################################################################
    #virtualenv --system-site-packages -p python2.7 ./venv # 原来pip的内容会复制过去
    #conda create -n venv pip python=2.7 # conda版本
    python -m venv venv/ # 完全从零开始


    source ./venv/bin/activate

    (venv) [root@centos test]# pip list # 这样就是自己的环境了

    deactivate # 退出当前环境
    ```
    2. conda
    ```
    conda create -n venv pip python=2.7 # conda版本
    
    source activate venv
    
    (venv) user@ubuntu:~/orris$ which python
    #---------------------------------------------------------------------------------
    # /home/user/anaconda2/envs/venv/bin/python
    #---------------------------------------------------------------------------------

    #pip install --ignore-installed --upgrade <packageURL> # 必须是pip包的完整路径
    pip install --ignore-installed --upgrade https://storage.googleapis.com/tensorflow/linux/gpu/tensorflow_gpu-1.11.0-cp27-none-linux_x86_64.whl
    conda install -n venv [package]
    
    source deactivate
    ```
