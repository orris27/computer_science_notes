# 安装Ubuntu后应该做的事

转载自[]浙大人工智能学生俱乐部公众号的同名文章](https://mp.weixin.qq.com/s/0acsbXpxnSUBPiQh2ka-dw)

本文主要想和大家分享一下，安装完Ubuntu后该怎样配置一个趁手的Ubuntu工作环境。这里面有推荐的软件，大家可以参考，根据自己的实际情况选择其中的安装和配置。以下操作都在Ubuntu16.04 LTS下完成，不同的版本操作大同小异，也可以作为参考。

## 切换国内软件源
使用国内的源能大幅提速，如果你是校园网的话还有教育网专线加成。推荐使用中科大的源，也可以使用清华的源，学生的话不建议其他的源，比如阿里云或者豆瓣的，第一是没有教育网速度加成，第二是这些镜像站有很多学术需求的项目没有维护，而中科大、或者清华的源维护的源就很全了，这里使用了中科大的源。具体细节可以查看中科大源帮助文档

### 中科大源帮助文档：
https://mirrors.ustc.edu.cn/help/
```
sudo sed -i 's/archive.ubuntu.com/mirrors.ustc.edu.cn/g' /etc/apt/sources.list
sudo apt-get update
```
或者直接编辑：
```
sudo gedit /etc/apt/sources.list
```
把里面的全部删掉换为:
```
# 默认注释了源码仓库，如有需要可自行取消注释
deb https://mirrors.ustc.edu.cn/ubuntu/ xenial main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ xenial main restricted universe multiverse
deb https://mirrors.ustc.edu.cn/ubuntu/ xenial-updates main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ xenial-updates main restricted universe multiverse
deb https://mirrors.ustc.edu.cn/ubuntu/ xenial-backports main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ xenial-backports main restricted universe multiverse
deb https://mirrors.ustc.edu.cn/ubuntu/ xenial-security main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ xenial-security main restricted universe multiverse
```

然后再运行：
```
sudo apt-get update
```



## 删除libreoffice，Amazon的链接
```
sudo apt-get remove libreoffice-common unity-webapps-common
```


## 删掉基本不用的自带软件
```
sudo apt-get remove thunderbird totem rhythmbox empathy brasero simple-scan gnome-mahjongg aisleriot gnome-mines cheese transmission-common gnome-orca webbrowser-app gnome-sudoku landscape-client-ui-install  
sudo apt-get remove onboard deja-dup
```

## 安装系统必备工具：Vim、git、axel、cmake、g++、curl
这些都是必备、好用的软件，建议都安装上。

+ 关于git我后面会详细写一份全程指南，到时候可供大家可以参考，更新后会把链接放这里。

+ vim 编辑器，vim指令要熟悉起来，学好了可以提高你码代码的速度。

+ axel 代替wget。

+ cmake 非常多的项目都要使用cmake编译。

+ g++ GNU的c++编译器，默认居然不带！

+ curl 利用URL语法在命令行方式下工作的开源文件传输工具
```
sudo apt-get install -y vim git cmake g++ axel curl
```
## 添加NVIDIA驱动源
### 官网：
http://blog.csdn.net/cosmoshua/article/details/76644029

如果你有NVIDIA的独立显卡，不管你是用于显示还是要装cuda进行深度学习，这样安装驱动是最安全的。后续我会写一份深度学习安装环境的配置指南，包括cuda安装，cudnn安装，tensorflow、pytorch、mxnet框架安装等。
```
sudo add-apt-repository ppa:xorg-edgers/ppa #添加ppa源
sudo add-apt-repository ppa:graphics-drivers/ppa #添加ppa源
sudo apt-get update #更新apt-get
```
## 安装teamviewer
远程桌面软件。
```
sudo dpkg -i Downloads/teamviewer*.deb
sudo apt-get -f install

```
## 安装Mendeley
看看论文啊。
```
sudo dpkg -i Downloads/mendeleydesktop*.deb
sudo apt-get -f install
```
## 安装pip
+ pip是强大有力的python包管理器，得到了python官方的支持。
```
sudo apt-get install -y python-pip python3-pip
```
+ 配置中科大的pypi源，配置好了下次pip install的时候就会自动使用中科大的源。
```
sudo vim /etc/pip.conf
```
+ 写入这些到文件中
```
[global]
index-url = https://mirrors.ustc.edu.cn/pypi/web/simple
format = columns
```
然后再执行
```
sudo pip2 install --upgrade pip
sudo pip3 install --upgrade pip
```
## 安装unity-tweak-tool 和主题
系统美化工具，效果如下：
![scheme](https://mmbiz.qpic.cn/mmbiz_png/hBIUUHOiaY6oy9b4w8zXyEqJAWU4djRgWHMEeVKPedjicWGF1URNqf1R6vDXXfKNhc1AWFT2LyNfvrFPQbBX2nRg/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)
```
sudo add-apt-repository ppa:noobslab/themes
sudo add-apt-repository ppa:noobslab/icons
sudo add-apt-repository ppa:noobslab/apps
sudo add-apt-repository ppa:noobslab/macbuntu
sudo apt-add-repository ppa:numix/ppa  
sudo add-apt-repository ppa:numix/ppa

sudo apt-get update 

sudo apt-get install -y notify-osd
sudo apt-get install -y unity-tweak-tool
sudo apt-get install -y numix-icon-theme-circle  
sudo apt-get install numix-gtk-theme numix-icon-theme-circle
#sudo apt-get install -y flatabulous-theme ultra-flat-icons

```
使用
```
unity-tweak-tool
```
+ 选择Appearance里面的Theme就能改变主题了
+ 默认Ubuntu的选项
    1. Theme: Ambiance
    2. Icons: Ubunutu-mono-dark
    3. Cursor: Dmz-white
## 安装系统指示器syspeek
看看网速啊，CPU占用啊等系统指标的
```
sudo add-apt-repository ppa:nilarimogard/webupd8
sudo apt-get update
sudo apt-get install -y syspeek
```
## 安装系统备份工具systemback
系统备份工具，非常好用。
```
sudo add-apt-repository ppa:nemh/systemback
sudo apt-get update
sudo apt-get install -y systemback unionfs-fuse
```


## 安装浙大校园网客户端
浙大的同学要想在Ubuntu下上网那就可以通过下面的命令实现，安装包可以在CC98找到，下载下来。

### 安装包网址：
http://www.cc98.org/topic/2323871
安装：
```
sudo dpkg --purge xl2tpd #确保xl2tpd没有被安装
sudo dpkg -i Downloads/xl2tpd*.deb#禁止更新xl2tpd,避免每次更新系统清除你的登录信息
echo "xl2tpd hold" | sudo dpkg --set-selections
```
## 安装搜狗输入法
### 方法1 
+ 安装完了网上有很多种方法安装，但是强烈推荐通过以下方式安装，因为默认的输入法框架ibus和Ubuntu桌面图形界面gnome的联系很紧，安装失败经常会导致整个gnome桌面系统崩溃。

+ 加入ubuntu kylin的apt源
```
sudo vim /etc/apt/sources.list.d/ubuntukylin.list
```
+ 写入:
```
deb http://archive.ubuntukylin.com:10006/ubuntukylin trusty main
```
+ 然后执行
```
sudo apt-get update && sudo apt-get install -y sogoupinyin
```
### 方法2
1. 去官网下载deb包
2. 安装
```
sudo dpkg --install sogoupinyin_2.2.0.0108_amd64.deb 
```
3. 如果报错depency不够的话,就修复
```
sudo apt-get --fix-broken install 
```
4. 重新安装
```
sudo dpkg --install sogoupinyin_2.2.0.0108_amd64.deb 
```
5. 在language support的keyboard input method system里选择fcitx
6. logout
7. 可以用了,shift就有了,右上角也有小键盘了

### 方法3
```
sudo apt-get install fcitx-googlepinyin 
reboot
```

## 安装微信
到 Github 下载最新包，这个是根据网页版微信用Electron封装的APP，Linux和MacOS都可以用，解压后进入软件目录,点击 electronic-wechat 后lock到启动栏就可以啦。比如我下载在`~/Downloads`下，所以只要启动`~/Downloads/electronic-wechat-linux-x64/electronic-wechat`

### Github微信：
https://github.com/geeeeeeeeek/electronic-wechat/releases

## 安装vscode
![vscode](https://mmbiz.qpic.cn/mmbiz_png/hBIUUHOiaY6oy9b4w8zXyEqJAWU4djRgWwTdibQ41RWVbMxyhnDydZ7Clia5CE88wkdhuJ1ve4JricibodNfw9UmkhQ/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

到 官网 下载最新安装包,写代码总得要有一个趁手的编辑器，我以前从sublime用到atom，现在基本只用vscode了，三大编辑器都有不少信徒，这里就不详细比较引起战争了，个人用得顺手就行了。不得不佩服巨硬深厚的开发功力。这篇教程就是vscode敲出来的，强烈推荐。

### vscode：
https://code.visualstudio.com/
```
sudo dpkg -i Downloads/code*.deb
```
## 安装WPS
Ubuntu下感觉就WPS好用一点。到 官网 下载最新安装包

### WPS：
http://wps-community.org/downloads
```
sudo dpkg -i Downloads/wps-office*.deb
sudo apt-get -f install
```
## 安装网易云音乐
到 官网 下载最新安装包

### 网易云音乐：
http://music.163.com/#/download
```
sudo dpkg -i Downloads/netease-cloud-music*.deb
```
## 安装有道词典
到 有道官网 下载最新安装包
 
### 有道词典：
http://cidian.youdao.com/index-linux.html
```
sudo dpkg -i Downloads/youdao*.deb
sudo apt-get -f install
```
## 安装Chrome
到 Chrome 官网 下载最新的安装文件。

### Chrome：
https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
```
sudo apt-get install libappindicator1 libindicator7
sudo dpkg -i Downloads/google-chrome-stable_current_amd64.deb
sudo apt-get -f install
```


## 安装shadowsocks-qt5
科学上网用的，就不多说了。到 Github 下载最新包

### shadowsocks-qt5：
https://github.com/shadowsocks/shadowsocks-qt5/releases

## Chrome 安装SwitchyOmega
浏览器设置代理的，也不多说，到 Github 下载最新包，鼠标拖入chrome的扩展中，就安装好了，然后具体的配置百度，或者email我，就不展开说了。

### SwitchyOmega：
https://github.com/FelisCatus/SwitchyOmega/releases

## 命令行代理工具polipo
详细配置参考：博客

### polipo：
http://blog.csdn.net/jesse_mx/article/details/52863204
```
sudo apt-get install -y polipo
```

## 安装docker-ce
docker大法好啊，谁用谁知道，你可以简单的把它看做一个超轻量的虚拟机，具体可以看官网的说明和教程吧。

### docker-ce：
https://docs.docker.com/
```
sudo apt-get install \
    apt-transport-https \
    ca-certificates \
    curl \
    software-properties-common

curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository \   "deb [arch=amd64] https://download.docker.com/linux/ubuntu \   $(lsb_release -cs) \
   stable"

sudo apt-get update && sudo apt-get install -y docker-ce
```
现在docker就装好啦，上面的操作是参考官方的Doc安装教程，当然由于我国国情，上面的方法不免很慢，所以你可以使用下面的命令体验一键极速安装：
```
curl -sSL https://get.daocloud.io/docker | sh
```
为了愉快地使用docker，可以执行以下操作：
```
#把docker加入用户组，这样每次使用docker命令就不用加sudo申请权限
sudo usermod -aG docker $USER
#使用daocloud加速镜像pull
curl -sSL https://get.daocloud.io/daotools/set_mirror.sh | sh -s http://e9aeed17.m.daocloud.io
```
如果要使用docker进行隔离式深度学习，那么在装好nvidia驱动和cuda后还可以安装nvidia-docker

### nvidia-docker：
https://github.com/NVIDIA/nvidia-docker

## 卸载firefox
已经装了chrome了，强迫症必须卸载掉！
```
sudo apt-get purge firefox*
```
## 编辑grub引导启动
```
sudo vim /etc/default/grub
```
编辑引导项，比如你要让grub默认引导时间为10秒，太长了，我就会修改以下项让引导时间变短：
```
GRUB_TIMEOUT = 0.1 #默认GRUB_TIMEOUT = 10
```
比如双系统默认引导到windows等，编辑完运行：
```
sudo update-grub
```
## 解决win10 和 Ubuntu 时间同步问题
如果你使用了windows和Ubuntu双系统，那么安装Ubuntu后会导致windows和Ubuntu出现8个小时的时间差，具体原因的话是Windows使用了UTC标准，Ubuntu没有，最简单解决方案就是使用ntpdate与标准时间同步，然后写入系统硬件时间。
```
sudo apt-get install -y ntpdate
sudo ntpdate time.nist.gov 
#sudo ntpdate time.windows.com
sudo hwclock --localtime --systohc
```
然后更新系统重启
```
sudo apt-get upgrade 
reboot
```
## 删除多余内核
使用正则表达式删除当前内核以外的所有多余内核，更新过后系统原来的内核不删除会导致/root空间越来越小，最后空间不足无法更新系统。
```
sudo dpkg -l 'linux-*' | sed '/^ii/!d;/'"$(uname -r | sed "s/\(.*\)-\([^0-9]\+\)/\1/")"'/d;s/^[^ ]* [^ ]* \([^ ]*\).*/\1/;/[0-9]/!d' | xargs sudo apt-get -y purge
```


## 省电
https://my.oschina.net/lxtesc/blog/115232
### tlp
!! LMT和tlp不能同时存在!


具体参考https://github.com/orris27/orris/blob/master/linux/shell/command.md
```
sudo add-apt-repository ppa:linrunner/tlp
sudo apt-get update
sudo apt-get install tlp tlp-rdw

sudo tlp start
sudo tlp-stat -s # if there is ENABLED, it is okay
```
### bluetooth
turn off

### WiFi
turn off

### screen brightness
调低

### Adobe Flash
Forbidden!

### 关闭独显
https://blog.csdn.net/Listener_ri/article/details/45290331
```
lspci | grep -i vga # rev off => okay
```


### Laptop Mode Tools

!! LMT和tlp不能同时存在! (apt-get安装的时候会自动removetlp)

Linux默认使用AC电源时最高性能（转速最快）

#### 1. Laptop Mode

Linux 有一个叫做 laptop_mode 的命令，这个命令即可以启用或者禁止“Laptop Mode”。所谓 Laptop Mode，是 Linux Kernel 2.4.23 和 2.6.6 以及更高版本中加入的一个特性，其目的主要是通过降低硬盘的转速来延长电池的续航时间。
```
sudo apt-get install laptop-mode-tools
sudo vim /etc/laptop-mode/laptop-mode.conf

#####################################################
#ENABLE_LAPTOP_MODE_ON_AC=0
ENABLE_LAPTOP_MODE_ON_AC=1 # 如果希望在 AC 供电的情况下也打开 Laptop Mode,就设置为1
#####################################################
```

验证
```
cat /proc/sys/vm/laptop_mode # 0表示未进入Laptop Mode.如果返回的是其他正数值则表示系统成功进入 Laptop Mode
# 也可以输入
sudo laptop_mode status
```
#### 2. 禁用CPU调节,让cpufreqd负责
不推荐安装多个CPU管理工具，可能会引起冲突，前面安装的硬盘调节工具也是具有CPU调节的功能的，所以要禁用掉,方法如下：
```
sudo vim /etc/laptop-mode/conf.d/cpufreq.conf

#####################################################
#CONTROL_CPU_FREQUENCY="auto"
CONTROL_CPU_FREQUENCY="0"
#####################################################

```
