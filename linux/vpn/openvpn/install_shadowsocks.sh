#! /bin/bash
# environment: CentOS 7.x
# shadowsocks-Python




define_consts()
{
    shadowsockspwd='123456'
    shadowsocksport='443'
    shadowsockscipher='aes-256-cfb'
    cur_dir="$HOME/tools/shadowsocks"
    application_dir='/application'
    shadowsocks_python_config='/etc/shadowsocks-python/config.json'
    shadowsocks_daemon="/usr/bin/ssserver"
    fast_open="true"
    shadowsocks_python_file="shadowsocks-master"

    [ ! -d ${cur_dir} ] && mkdir -p $cur_dir
    [ ! -d ${application_dir} ] && mkdir -p $application_dir
    [ ! -d "$(dirname ${shadowsocks_python_config})" ] && mkdir -p "$(dirname ${shadowsocks_python_config})"
}



disable_selinux()
{
    if [ -s /etc/selinux/config ] && grep 'SELINUX=enforcing' /etc/selinux/config; then
        sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config
        setenforce 0
    fi
}

install_git()
{
    yum install curl-devel expat-devel gettext-devel openssl-devel zlib-devel gcc perl-ExtUtils-MakeMaker autoconf asciidoc xmlto docbook2X autoconf wget -y
    ln -s /usr/bin/db2x_docbook2texi /usr/bin/docbook2x-texi

    cd $cur_dir

    wget https://mirrors.edge.kernel.org/pub/software/scm/git/git-2.18.0.tar.gz
    tar -zxf git-2.18.0.tar.gz
    cd git-2.18.0

    make configure
    ./configure --prefix=/application/git-2.18.0
    make all doc info
    make install install-doc install-html install-info

    ln -s /application/git-2.18.0/ /application/git
    echo 'export PATH=/application/git/bin:$PATH' >> /etc/profile
    source /etc/profile

}

install_dependencies()
{
    yum install -y epel-release yum-utils > /dev/null 2>&1
    [ x"$(yum-config-manager epel | grep -w enabled | awk '{print $3}')" != x"True" ] && yum-config-manager --enable epel > /dev/null 2>&1
    install_git
    yum install -y unzip gzip openssl openssl-devel gcc python python-devel python-setuptools pcre pcre-devel libtool libevent autoconf automake make curl curl-devel zlib-devel perl perl-devel cpio expat-devel gettext-devel libev-devel c-ares-devel qrencode > /dev/null 2>&1
}

config_shadowsocks()
{
    cat > ${shadowsocks_python_config}<<-EOF
{
    "server":"0.0.0.0",
    "server_port":${shadowsocksport},
    "local_address":"127.0.0.1",
    "local_port":1080,
    "password":"${shadowsockspwd}",
    "timeout":300,
    "method":"${shadowsockscipher}",
    "fast_open":${fast_open}
}
EOF
}

config_firewall()
{
    systemctl status firewalld > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        firewall-cmd --permanent --zone=public --add-port=${shadowsocksport}/tcp
        firewall-cmd --permanent --zone=public --add-port=${shadowsocksport}/udp
        firewall-cmd --reload
    fi
}


install_libsodium()
{
    if [ ! -f /usr/lib/libsodium.a ]; then
        cd ${cur_dir}
        wget --no-check-certificate -c -t3 -T60 -O "libsodium-1.0.16.tar.gz" "https://github.com/jedisct1/libsodium/releases/download/1.0.16/libsodium-1.0.16.tar.gz"

        tar zxf "libsodium-1.0.16.tar.gz"
        cd "libsodium-1.0.16"
        ./configure --prefix=/usr && make && make install
    fi
}

install_main()
{
    install_libsodium
    if ! ldconfig -p | grep -wq "/usr/lib"; then
        echo "/usr/lib" > /etc/ld.so.conf.d/lib.conf
    fi
    ldconfig

    cd ${cur_dir}
    wget --no-check-certificate -c -t3 -T60 -O "shadowsocks-master.zip" "https://github.com/shadowsocks/shadowsocks/archive/master.zip"
    unzip -q ${shadowsocks_python_file}.zip
    cd ${shadowsocks_python_file}
    python setup.py install --record /usr/local/shadowsocks_python.log

}



start_service()
{
    sed -i 's/#!\/usr\/bin\/python/#!\/usr\/bin\/python2.7/g' /usr/bin/ssserver

    ssserver -c ${shadowsocks_python_config} -d start
}


define_consts

disable_selinux

install_dependencies

config_shadowsocks

config_firewall

install_main

start_service
