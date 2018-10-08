    
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
        + 实际查询的sys.path是`['/home/orris/fun/test', '/home/orris/anaconda3/envs/orris/lib/python27.zip', '/home/orris/anaconda3/envs/orris/lib/python2.7', '/home/orris/anaconda3/envs/orris/lib/python2.7/plat-linux2', '/home/orris/anaconda3/envs/orris/lib/python2.7/lib-tk', '/home/orris/anaconda3/envs/orris/lib/python2.7/lib-old', '/home/orris/anaconda3/envs/orris/lib/python2.7/lib-dynload', '/home/orris/.local/lib/python2.7/site-packages', '/home/orris/anaconda3/envs/orris/lib/python2.7/site-packages']`,也就是说除了`.`和`.local`下的内容外就是环境下的包
    ```
    conda create -n venv pip python=2.7 # conda版本
    
    source activate venv
    
    (venv) user@ubuntu:~/orris$ which python
    #---------------------------------------------------------------------------------
    # /home/user/anaconda2/envs/venv/bin/python
    #---------------------------------------------------------------------------------

    conda install -n venv <package> # 这里填写package的名字就行了."-n"表示指定环境
    
    source deactivate
    
    
    conda remove -n your_env_name(虚拟环境名称) --all # 删除虚拟环境
    ```
