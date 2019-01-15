## 1. 开机
1. 按下电源键,主板加电
2. BIOS自检
3. BIOS根据引导顺序(就是我们设定的1~5)查找可以引导的介质,从1开始找到5直到找到为止
4. 找到介质后,在0盘片0磁道1扇区上的前512个字节内找到主引导记录
5. 主引导记录会加载grub
6. grub会启动镜像和后续加载

## 2. 省电
### 2-1. 关闭蓝牙
点击电池 > PowerSetting > Bluetooth关闭
### 2-2. 禁用独显(没成功)
```
sudo add-apt-repository ppa:bumblebee/stable
sudo apt-get update

sudo apt install bumblebee-nvidia bumblebee primus

sudo apt-get install mesa-utils

lspci |grep -i vga # rev ff表示独显已经关闭,如果是数字,说明独显处于开启状态
#----------------------------------------------------------------------------------
# 00:02.0 VGA compatible controller: Intel Corporation HD Graphics 530 (rev 06)
#----------------------------------------------------------------------------------


glxgears -info
#----------------------------------------------------------------------------------
# Running synchronized to the vertical refresh.  The framerate should be
# approximately the same as the monitor refresh rate.
# GL_RENDERER   = GeForce GTX 965M/PCIe/SSE2
# GL_VERSION    = 4.6.0 NVIDIA 396.26
# GL_VENDOR     = NVIDIA Corporation
# ...
#----------------------------------------------------------------------------------
```
## 3. UltraISO
https://www.jb51.net/softjc/61771.html
