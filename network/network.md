## 1. ens33 => eth0
1. 修改`/etc/sysconfig/network-scripts/ifcfg-ens33`中的Name和Device
```
sed -i 's/ens33/eth0/g' /etc/sysconfig/network-scripts/ifcfg-ens33
```

2. 重命名`ifcfg-ens33`
```
mv /etc/sysconfig/network-scripts/ifcfg-ens33 /etc/sysconfig/network-scripts/ifcfg-eth0
```



3. 修改`/etc/defualt/grub`
+ 加入"net.ifnames=0 biosdevname=0"到GRUB_CMDLINE_LINUX变量
```
sed -i '/GRUB_CMDLINE_LINUX/s/"$/\ net.ifnames=0 biosdevname=0"/g' /etc/default/grub
```


4. 运行命令`grub2-mkconfig -o /boot/grub2/grub.cfg`来重新生成GRUB配置并更新内核参数
```
grub2-mkconfig -o /boot/grub2/grub.cfg
```

5. 重启系统
```
reboot
```

6. 验证
```
ifconfig
```
