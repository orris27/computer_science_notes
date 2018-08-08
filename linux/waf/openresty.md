```
yum install -y readline-devel pcre-devel openssl-devel
cd /usr/local/src
wget https://openresty.org/download/openresty-1.13.6.2.tar.gz

tar zxf openresty-1.13.6.2.tar.gz
cd openresty-1.13.6.2
./configure --prefix=/usr/local/openresty-1.13.6.2 \
--with-luajit --with-http_stub_status_module \
--with-pcre --with-pcre-jit

gmake && gmake install
ln -s /usr/local/openresty-1.13.6.2 /usr/local/openresty 

vim /usr/local/openresty/nginx/conf/nginx.conf
###########################################################
server {
    location /hello {
            default_type text/html;
            content_by_lua_block {
                ngx.say("HelloWorld")
            }
        }
}
###########################################################

/usr/local/openresty/nginx/sbin/nginx -t
/usr/local/openresty/nginx/sbin/nginx

curl http://192.168.56.10/hello
```
