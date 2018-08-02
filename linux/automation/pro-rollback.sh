#! /bin/bash

usage(){

	echo "$Usage: $0 [ list | version ]"
}

rollback_pro(){
	rm -f /var/www/html/code && ln -s $1 /var/www/html/code

}


main(){
	if [ $# -ne 1 ]
	  then
		  usage
	fi
	case $1 in
		list)
			ls -l /tmp/pro | grep ^d
		;;
	    *)
			rollback_pro $1
		exit 1
	esac
}


main $1
