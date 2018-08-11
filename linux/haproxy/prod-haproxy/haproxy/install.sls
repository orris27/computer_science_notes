include:
  - pkg.pkg-init

haproxy-install:
  file.managed:
    - name: /root/tools/haproxy-1.8.12.tar.gz
    - source: salt://haproxy/files/haproxy-1.8.12.tar.gz
    - user: root
    - group: root
    - mode: 755
  cmd.run:
    - name: cd /root/tools && tar -zxf haproxy-1.8.12.tar.gz && cd haproxy-1.8.12 && sudo make TARGET=linux2628 PREFIX=/usr/local/haproxy-1.8.12 && sudo make install PREFIX=/usr/local/haproxy-1.8.12 && sudo ln -s /usr/local/haproxy-1.8.12/ /usr/local/haproxy
    - unless: test -d /usr/local/haproxy
    - require:
      - pkg: pkg-init
      - file: haproxy-install

haproxy-init:
  file.managed:
    - name: /etc/init.d/haproxy
    - source: salt://haproxy/files/haproxy.init
    - user: root
    - group: root
    - mode: 755
    - require:
      - cmd: haproxy-install
  cmd.run:
    - name: chkconfig --add haproxy
    - unless: chkconfig --list | grep haproxy
    - require:
      - file: haproxy-init


haproxy-config-dir:
  file.directory:
    - name: /etc/haproxy
    - user: root
    - group: root
    - mode: 755

command-link:
  cmd.run:
    - name: ln -s /usr/local/haproxy/sbin/haproxy  /usr/local/bin/
    - require:
      - file: haproxy-config-dir
