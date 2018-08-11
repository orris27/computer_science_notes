## 1. 反向代理
```
cd /application/apache/conf/extra

vim httpd-proxy.conf
#########################################################################
LoadModule proxy_module modules/mod_proxy.so
LoadModule proxy_http_module modules/mod_proxy_http.so
LoadModule proxy_balancer_module modules/mod_proxy_balancer.so
LoadModule lbmethod_byrequests_module modules/mod_lbmethod_byrequests.so
LoadModule lbmethod_bytraffic_module modules/mod_lbmethod_bytraffic.so
LoadModule lbmethod_bybusyness_module modules/mod_lbmethod_bybusyness.so
LoadModule slotmem_shm_module modules/mod_slotmem_shm.so

ProxyRequests Off
<Proxy balancer://mycluster>
BalancerMember http://10.0.0.8:80
BalancerMember http://10.0.0.9:80
</Proxy>



<Location /manager>
    SetHandler balancer-manager
    Order Deny,Allow
    Allow from all
</Location>


<VirtualHost *:80>
    ServerAdmin webmaster@dummy-host.example.com
    DocumentRoot "/var/html/www"
    ServerName www.orris.com
    ServerAlias orris.com
    ErrorLog "logs/www.orris.com-error_log"
    CustomLog "logs/www.orris.com-access_log" common
    ProxyPass /demo balancer://mycluster
    ProxyPassReverse /demo balancer://mycluster
</VirtualHost>
#########################################################################

cd ..
vim httpd.conf
#########################################################################
#proxy demo
Include conf/extra/httpd-proxy.conf
#########################################################################

apachectl -t
apachectl -k start

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 在10.0.0.{8,9}上也要允许/demo的URI,所以创建对应的demo目录和index.html
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

echo "10.0.0.7 www.orris.com" >> /etc/hosts
curl www.orris.com/demo

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 浏览器可以访问10.0.0.7/mananger进行负载均衡的管理
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



```