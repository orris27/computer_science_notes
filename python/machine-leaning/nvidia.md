# NVIDIA显卡深度学习环境配置教程
转载自[浙大人工智能学生俱乐部公众号的同名文章](https://mp.weixin.qq.com/s/vPQKmrZaiCfQxZknbI5ZrA)
## 前言
本文为NVIDIA显卡深度学习环境配教程，主要内容为在ubuntu环境下安装NVIDIA显卡驱动、CUDA以及cuDNN。前面如何配置好Ubuntu基本工作环境请参考 安装Ubuntu后应该做的事 ,本文提供一种可行稳妥的安装方法，更详细的安装方式还可参考CUDA官方安装文档与cuDNN官方安装文档。以下操作均在ubuntu 16.04.4 下测试通过，其他发行版本可作参考。

### CUDA官方安装文档：
https://docs.nvidia.com/cuda/cuda-installation-guide-linux/

### cuDNN官方安装文档：
https://docs.nvidia.com/deeplearning/sdk/cudnn-install/index.html

## 安装NVIDIA显卡驱动
NVIDIA显卡驱动一般有三种安装方法:

    1. 使用PPA源安装：最安全，最方便。但是未必有最新驱动，而且下载速度可能慢一点，需要准备好梯子。

    2. 安装CUDA时，顺便安装驱动，但未必是最新驱动（不推荐）。

    3. 官网下载NVIDIA驱动程序然后本地安装（下载runfile）。

### 1. apt-get安装（推荐)
#### 卸载原有驱动
+ 若已有N卡驱动，则需要先卸载：
```
sudo apt-get remove --purge nvidia*
```
+ 如果是runfile安装:
```
sudo chmod +x NVIDIA*.run
sudo ./NVIDIA-Linux-x86_64*.run –uninstall
```
#### 安装NVIDIA驱动
然后执行：
```
sudo add-apt-repository ppa:xorg-edgers/ppa #添加ppa源
sudo add-apt-repository ppa:graphics-drivers/ppa #添加ppa源
sudo apt-get update #更新apt-get
# Software & Updates > Additional Drivers > check the nvidia version
# Mine is nvidia-396
# 参考资料上写的是sudo apt-get install nvidia-390 #安装390版本的driver
sudo apt-get install nvidia-396 # 执行到这里后面只要sudo apt-get install cuda-9-0;reboot;就好了的样子
```
然后可能下载较慢但是最安全，这样安装系统会自动帮你禁用nouveau，所以这样安装完后不用手动禁用nouveau。

### 2. runfile 安装
去NVIDIA驱动程序下载官网下载合适的最新驱动，然后本地安装（下载runfile）。
    + NVIDIA驱动程序下载官网：`https://www.nvidia.cn/Download/index.aspx?lang=cn`

1. 查看GPU型号
```
lspci | grep -i nvidia
#-------------------------------------------------------------------------------------
# 01:00.0 3D controller: NVIDIA Corporation GM206M [GeForce GTX 965M] (rev ff)
#-------------------------------------------------------------------------------------
```
2. GPU具体信息:(貌似它让我下载410版本的驱动,可是我安装失败了,396和390都失败了)

![gpu_type](https://github.com/orris27/orris/raw/master/images/gpu_type.png)


### 禁用Nouveau驱动
打开配置文件：
```
sudo vim /etc/modprobe.d/blacklist.conf
```
然后在文本最后添加(禁用nouveau第三方驱动，之后也不需要改回来)：
```
blacklist nouveau
options nouveau modeset=0
```
然后执行：
```
sudo update-initramfs -u
```
重启后，执行：
```
lsmod | grep nouveau
```
如果没有屏幕输出，说明禁用nouveau成功。
### 命令行安装驱动

在NVIDIA驱动程序下载官网下载合适的最新驱动，然后按下Ctrl+Alt+F1进入命令行模式，进入驱动所在文件夹，输入以下命令：
```
sudo service lightdm stop #这会关闭图形界面(X server)，但不用紧张
# warning: Do not use "sudo systemctl stop gdm;sudo systemctl start gdm;".这会导致循环登录,当然重启就解决这个问题了



sudo chmod +x NVIDIA-Linux-x86_64-*.run #给驱动run文件赋予执行权限
sudo ./NVIDIA-Linux-x86_64-*.run -no-opengl-files #后面的参数非常重要，不可省略
#sudo ./NVIDIA-Linux-x86_64-*.run -no-opengl-files -no-x-check # 如果不关闭xw服务的话
```
+ -no-opengl-files：表示只安装驱动文件，不安装OpenGL文件。这个参数不可省略，否则会导致登陆界面死循环。

+ -no-x-check：表示安装驱动时不检查X服务，非必需。

+ -no-nouveau-check：表示安装驱动时不检查nouveau，非必需。

+ -Z, —disable-nouveau：禁用nouveau。此参数非必需，因为之前已经手动禁用了nouveau。

+ -A：查看更多高级选项

必选参数解释：因为NVIDIA的驱动默认会安装OpenGL，而Ubuntu的内核本身也有OpenGL、且与GUI显示息息相关，一旦NVIDIA的驱动覆写了OpenGL，在GUI需要动态链接OpenGL库的时候就引起问题。

之后，按照提示安装，成功后重启即可。 如果提示安装失败，不要急着重启电脑，重复以上步骤，多安装几次即可。

如果使用`-no-x-check`(至少我使用了),并且提示:
```
The distribution-provided pre-install script failed!  Are you sure you want to continue?
```


#### NVIDIA驱动程序下载官网：
https://www.nvidia.cn/Download/index.aspx?lang=cn

### 测试
```
nvidia-smi #若列出GPU的信息列表，表示驱动安装成功
nvidia-settings #若弹出设置对话框，亦表示驱动安装成功
```
![NVIDIA test](https://mmbiz.qpic.cn/mmbiz_jpg/hBIUUHOiaY6oy9b4w8zXyEqJAWU4djRgWsewKQSia2XPcFn81RpAfG5S8kN0Puic2Mibrh3xNO1zIZnUn9z0S2ticbg/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)
## 安装CUDA
到CUDA官方下载页面下载CUDA包(runfile)
![CUDA website](https://github.com/orris27/orris/raw/master/images/cuda-website.png)

### CUDA官方下载页面：
https://developer.nvidia.com/cuda-downloads

建议安装版本：cuda 9.0 （tensorflow 现在支持得最好的cuda版本） => 直接在google里搜索cuda tookit 9.0

然后按照以下步骤安装：

### 安装
貌似直接使用`sudo apt-get install cuda-9-0;reboot;`就可以了

首先确保文件具有运行权限，然后运行：
```
sudo ./cuda_*linux.run --no-opengl-libs
```
+ —no-opengl-libs：表示只安装驱动文件，不安装OpenGL文件。必需参数，原因同上。注意：不是-no-opengl-files。

+ —toolkit：表示只安装CUDA Toolkit，不安装Driver和Samples。

+ —help：查看更多高级选项。

之后，按照提示安装即可。依次选择：
```
accept #同意安装
n #不安装Driver，因为已安装最新驱动
y #安装CUDA Toolkit
<Enter> #安装到默认目录
y #创建安装目录的软链接
n #不复制Samples，因为在安装目录下有/samples
```
### CUDA Sample测试
```
#编译并测试设备 deviceQuery：
cd /usr/local/cuda-9.0/samples/1_Utilities/deviceQuery
sudo make
```
然后执行：
```
./deviceQuery
```
如果测试的最后结果是Result = PASS，说明CUDA安装成功。

### 把CUDA加到环境变量中
```
vim ~/.bashrc
```
加入以下变量：
```
export PATH=/usr/local/cuda-9.0/bin${PATH:+:${PATH}} 
export LD_LIBRARY_PATH=/usr/local/cuda-9.0/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
```
使设置生效：
```
source ~/.bashrc
```
## 安装cuDNN
到cuDNN官方下载页面下载与CUDA版本相匹配的cuDNN包，要注册一个Nvidia账号，然后才可以下载。

建议的版本：cudnn 7.0 for cuda 9.0
![cudnn](https://github.com/orris27/orris/blob/master/images/cudnn.png?raw=true)


### cuDNN官方下载页面：
https://developer.nvidia.com/cudnn

进入安装包所在目录，依次输入以下命令：
```
tar -xzvf cudnn-9.0-linux-x64-v7.tgz
sudo cp cuda/include/cudnn.h /usr/local/cuda/include
sudo cp cuda/lib64/libcudnn* /usr/local/cuda/lib64
sudo chmod a+r /usr/local/cuda/include/cudnn.h /usr/local/cuda/lib64/libcudnn*
```
至此，关于NVIDIA显卡环境的一切已全部配置完成。

## 安装 Tensorflow GPU 版本
关于GPU加速训练过程不再强调，推荐tensorflow 1.6 版本，刚好和cuda对应起来，非常稳定。
```
sudo pip install tensorflow-gpu==1.6 --user #python2
sudo pip3 install tensorflow-gpu==1.6 --user #python3
```
## 安装 Pytorch GPU 版本
到Pytorch官网安装最新版本就可以了，pytorch用户体验很好，一般没有兼容性问题。选择你的Python版本和安装方式，推荐使用pip安装，选好cuda 9.0，然后官网会生成安装命令，copy到命令行执行即可安装。
![pytorch](https://mmbiz.qpic.cn/mmbiz_png/hBIUUHOiaY6rN5FbeWiaYMtE0IhgT3VuTj5lEypLQVyjdNmSf1Z24gP1NHDqvj6FEe5yqFX1ibsxCLoBgJu5o899Q/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)


### Pytorch官网：
https://pytorch.org/

## 安装 MxNet GPU 版本
参考MxNet官网安装教程安装最新版本就可以了。

### MxNet官网安装教程：
https://mxnet.incubator.apache.org/install/index.html

+ 安装依赖
```
sudo apt-get update
sudo apt-get install -y wget python
wget https://bootstrap.pypa.io/get-pip.py && sudo python get-pip.py
```
+ 安装cuda 9.0 版
```
pip install mxnet-cu90 --pre
```
其他有一些可选安装项可以看MxNet官网安装选择性安装。

### MxNet官网安装：
https://mxnet.incubator.apache.org/install/index.html

