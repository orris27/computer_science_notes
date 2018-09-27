    
## 1. Linux内核编译
```

uname -r
#--------------------------------------------
# 3.10.0-862.el7.x86_64
#--------------------------------------------

mkdir ~/tools
cd ~/tools

wget http://mirrors.aliyun.com/linux-kernel/v4.x/linux-4.13.3.tar.gz
tar -xvf linux-4.13.3.tar.gz
cd linux-4.13.3
cp /usr/src/linux-headers-4.10.0-38-generic/.config .



#make menuconfig
make

make modules_install
make install
cat /boot/grub/grub.cfg
reboot
uname -r
```
