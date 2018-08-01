## 1. 挖矿病毒qW3xT.2和ddgs.3013
> http://blog.51cto.com/legehappy/2152838
1. 原因
    1. 6379端口暴露在外网
    2. Redis没有设置密码
2. 解决
    1. 利用防火墙关闭6379端口
    2. 清空crontab定时任务
    3. 杀掉qW3xT.2和ddgs.3013进程
    4. 删除/tmp下的这两个文件
```
sudo iptables -L -n 
sudo systemctl status iptables
iptables -t filter -A INPUT -p tcp --dport 6379 -j DROP
vim  /usr/local/redis/redis.conf
crontab -l
echo > /var/spool/cron/root 
crontab -l
top
kill 6594
top
ps -aux | grep ddg
kill 6097
ps -axu | egrep 'ddg|qW'
rm -rf /tmp/ddgs.3013 /tmp/qW3xT.2 
```
