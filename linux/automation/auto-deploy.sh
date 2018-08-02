#! /bin/bash

CODE_DIR=/deploy/source-code
CODE_CONFIG=/deploy/config
TMP_DIR=/deploy/tmp
CTIME=$(date +%F-%H-%M)

get_code(){
	ver=$1
	GIT_CID=$(cd $CODE_DIR && git log | head -1 | cut -d ' ' -f2)
	GIT_SID=$(echo ${GIT_CID:0:5})
	if [ -z $ver ]
	  then
		  cd $CODE_DIR/deploy-demo && git pull
	  else
		  cd $CODE_DIR/deploy-demo && git checkout $ver
	fi
	/bin/cp -r "$CODE_DIR"/deploy-demo "$TMP_DIR"/deploy-demo
}

build_code(){
	echo "build code ok"

}

pkg_code(){
	/bin/cp "$CODE_CONFIG" "$TMP_DIR"/deploy-demo
	PKG_VER="${GIT_SID}_${CTIME}"
	cd "$TMP_DIR" && mv deploy-demo pro-deploy-demo-${PKG_VER}
	cd "$TMP_DIR" && tar czf pro-deploy-demo-${PKG_VER}.tar.gz pro-deploy-demo-${PKG_VER}
}

scp_code(){
	scp "$TMP_DIR"/pro-deploy-demo-${PKG_VER}.tar.gz 192.168.1.2:/tmp

}

node_remove(){
	echo "remove ok"

}

link_code(){
	cd /tmp && tar zxf pro-deploy-demo-${PKG_VER}.tar.gz
	rm -f /var/www/html/code && ln -s /tmp/pro-deploy-demo-${PKG_VER} /var/www/html/code

}

diff_config(){
	echo "ok"

}

reload_code(){
	echo "ok"
}

curl_service(){
	HTTP_CODE=$(curl --head http://192.168.1.2/code/index.html | grep 200)

	if [ -z "$HTTP_CODE" ]
	  then
		  echo "bad and exit"
		  rm -f /var/run/deploy-start.lock
		  exit
		
	fi
}

node_add(){
	echo "ok"

}

main(){
	if [ -f /var/run/deploy-start.lock ]
	  then
		  exit
	fi
	touch /var/run/deploy-start.lock
	code_ver=$1
	get_code $code_ver
	build_code
	pkg_code
	scp_code
	node_remove
	link_node
	diff_config
	reload_code
	curl_service
	node_add
	rm -f /var/run/deploy-start.lock
}

main $1

