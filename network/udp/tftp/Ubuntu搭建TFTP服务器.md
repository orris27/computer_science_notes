# Ubuntu搭建TFTP服务器

## 安装

```shell
sudo apt-get install xinetd # TFTP依赖网络守护进程服务程序
sudo apt-get install tftp tftpd
```

## 建立配置文件

在/etc/xinetd.d/目录下新建tftp文件

写入如下内容

```
service tftp  
{  
     socket_type    = dgram  
     protocol       = udp  
     wait           = yes  
     user           = root  
     server         = /usr/sbin/in.tftpd  
     server_args    = -s /home/orris/dir1/
     disable        = no  
     per_source     = 11  
     cps            = 100 2  
     flags          = IPv4  
}

```

(server_args的目录就是之后tftp服务器的目录)

设置/home/orris/dir1这个目录的访问权限为777

```shell
chmod 777 /home/orris/dir1
```

## 启动服务器

```shell
sudo /etc/init.d/xinted start # 如果刚修改了配置文件,用restart
```

## 检查服务器是否启动

```shell
sudo netstat -a | grep tftp
```

## 新建和获取文件

新建文件

```shell
touch /home/orris/dir1/1.txt
```

获取文件

```shell
tftp 127.0.0.1 # 本机测试
tftp > get 1.txt
```



