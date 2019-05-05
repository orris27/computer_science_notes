# NVIDIA显卡深度学习环境配置教程
参考自[浙大人工智能学生俱乐部公众号的同名文章](https://mp.weixin.qq.com/s/vPQKmrZaiCfQxZknbI5ZrA)
## Objectiveness
1. NVIDIA driver
2. CUDA

## NVIDIA driver
NVIDIA显卡驱动一般有三种安装方法:

1. 使用PPA源安装：最安全，最方便。但是未必有最新驱动，而且下载速度可能慢一点，需要准备好梯子。

2. 安装CUDA时，顺便安装驱动，但未必是最新驱动。

3. 官网下载NVIDIA驱动程序然后本地安装（下载runfile）。

### 1. PPA
#### 卸载
+ 若已有N卡驱动，则需要先卸载：
```
sudo apt-get remove --purge nvidia*
```
+ 如果是runfile安装:
```
sudo chmod +x NVIDIA*.run
sudo ./NVIDIA-Linux-x86_64*.run –uninstall
```
#### 安装
然后执行：
```
sudo add-apt-repository ppa:xorg-edgers/ppa #添加ppa源
sudo add-apt-repository ppa:graphics-drivers/ppa #添加ppa源
sudo apt-get update #更新apt-get
# Software & Updates > Additional Drivers > check the nvidia version


sudo apt-get install nvidia-396 # 执行到这里后面只要sudo apt-get install cuda-9-0;reboot;就好了的样子
```
然后可能下载较慢但是最安全，这样安装系统会自动帮你禁用nouveau，所以这样安装完后不用手动禁用nouveau。

### 2. With CUDA

### 3. Runfile
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


#### 禁用Nouveau驱动
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
#### 命令行安装驱动

在NVIDIA驱动程序下载官网下载合适的最新驱动，然后按下Ctrl+Alt+F1进入命令行模式，进入驱动所在文件夹，输入以下命令：
```
sudo service lightdm stop #这会关闭图形界面(X server)，但不用紧张
# warning: Do not use "sudo systemctl stop gdm;sudo systemctl start gdm;".这会导致循环登录,当然重启就解决这个问题了



sudo chmod +x NVIDIA-Linux-x86_64-*.run #给驱动run文件赋予执行权限
sudo ./NVIDIA-Linux-x86_64-*.run -no-opengl-files #后面的参数非常重要，不可省略
#sudo ./NVIDIA-Linux-x86_64-*.run -no-opengl-files -no-x-check # 如果不关闭xw服务的话(不推荐)
```
+ -no-opengl-files：表示只安装驱动文件，不安装OpenGL文件。这个参数不可省略，否则会导致登陆界面死循环。

+ -no-x-check：表示安装驱动时不检查X服务，非必需。

+ -no-nouveau-check：表示安装驱动时不检查nouveau，非必需。

+ -Z, -disable-nouveau：禁用nouveau。此参数非必需，因为之前已经手动禁用了nouveau。

+ -A：查看更多高级选项

必选参数解释：因为NVIDIA的驱动默认会安装OpenGL，而Ubuntu的内核本身也有OpenGL、且与GUI显示息息相关，一旦NVIDIA的驱动覆写了OpenGL，在GUI需要动态链接OpenGL库的时候就引起问题。

之后，按照提示安装，成功后重启即可。 如果提示安装失败，不要急着重启电脑，重复以上步骤，多安装几次即可。

如果使用`-no-x-check`(至少我使用了),并且提示:
```
The distribution-provided pre-install script failed!  Are you sure you want to continue?
```
#### Common Installation Problems
1. 输入`nvidia-smi`,报错如下所示

NVIDIA-SMI has failed because it couldn't communicate with the NVIDIA driver. Make sure that the latest NVIDIA driver is installed and running.

执行`sudo /media/orris/DATA/dataset/NVIDIA-Linux-x86_64-396.54.run -no-opengl-files -no-x-check`的时候出错.按照提示找到错误输出日志`/var/lib/dkms/nvidia/410.78/build/make.log`,发现说
```
gcc version 7.2.0 (Ubuntu 7.2.0-8ubuntu3.2)

does not match the compiler used here:

cc (Ubuntu 6.4.0-8ubuntu1) 6.4.0 20171010
Copyright (C) 2017 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

```
貌似gcc的版本不正确,执行`gcc --version`发现我的gcc是6.4.0版本的,可能需要安装7.2.0版本的.



2. 执行`sudo /media/orris/DATA/dataset/NVIDIA-Linux-x86_64-396.54.run -no-opengl-files -no-x-check`的时候出错,发现说
```
Unable to load the 'nvidia-drm' kernel module.
Installation has failed.  Please see the file '/var/log/nvidia-installer.log' for details.  You may find suggestions on fixing installation problems in the README available on the Linux driver download page at www.nvidia.com.
```
执行`nvidia-smi`发现出现界面了,所以好像没问题的样子


#### NVIDIA驱动程序下载官网：
https://www.nvidia.cn/Download/index.aspx?lang=cn




## CUDA(推荐apt安装)
### CUDA官方安装文档：
https://docs.nvidia.com/cuda/cuda-installation-guide-linux/

### cuDNN官方安装文档：
https://docs.nvidia.com/deeplearning/sdk/cudnn-install/index.html



## Summary
### Environment
+ Ubuntu: 17.10
+ Nvidia driver version: 396.26
+ CUDA: 9.0
```
Ctrl + Alt + F4 # 在命令行中进行
sudo apt-get remove --purge nvidia*
sudo ./NVIDIA-Linux-x86_64*.run –uninstall

ps aux| grep gdm # 如果有显示说明正在运行,关闭掉
systemctl stop gdm

sudo apt-get install cuda-9-0 # 安装cuda-9-0的时候内部直接安装了nvidia-396,而不需要手动runfile安装
reboot
```
### Check if installed successfully
#### 1. Nvidia Driver
```
nvidia-smi #若列出GPU的信息列表，表示驱动安装成功
nvidia-settings #若弹出设置对话框，亦表示驱动安装成功
```
![NVIDIA test](https://mmbiz.qpic.cn/mmbiz_jpg/hBIUUHOiaY6oy9b4w8zXyEqJAWU4djRgWsewKQSia2XPcFn81RpAfG5S8kN0Puic2Mibrh3xNO1zIZnUn9z0S2ticbg/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

#### 2. CUDA    
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
