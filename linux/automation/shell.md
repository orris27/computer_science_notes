## 1. 使用shell编写基于etcd的SaltStack实现haproxy扩容的脚本
1. 编写nginx的状态文件
```
curl -s --head http://172.19.28.82/ | grep '200 OK'
cd /srv/salt/prod/
mkdir nginx
cd nginx/

cat > install.sls <<EOF
nginx-install:
  pkg.installed:
    - name: nginx
  service.running:
    - name: nginx
    - enable: True
    - reload: True
EOF

salt '*' state.sls nginx.install env=prod
```
2. 脚本文件
```
#! /bin/sh

create_host() {
	echo "create host ok"
}

deploy_service(){
	salt 'linux-node1.example.com' state.sls nginx.install env=prod
}

deploy_code(){
	echo "deploy code ok"

}
service_check(){
	STATUS=$( curl -s --head http://172.19.28.82/ | grep '200 OK')
	if [ -n "$STATUS" ]
	then
		echo "ok"
	else
		echo "not ok"
		exit
	fi
}

etcd_key(){
	curl -s http://172.19.28.82:2379/v2/keys/salt/haproxy/backend_www_oldboyedu_com/web-node1 -XPUT -d value="172.19.28.82:8080" | python -m json.tool
}


sync_state(){
	salt 'linux-node1.example.com' state.sls cluster.haproxy-outside env=prod
}


main(){
	create_host
	deploy_service
	deploy_code
}
```
