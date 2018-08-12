#! /bin/bash

TOMCAT_PATH=/usr/local/tomcat
usage () {
    echo "Usage: $0 [start|stop|status|restart]"
}

start () {
    /usr/local/tomcat/bin/startup.sh
}


stop () {
    TPID=$(ps aux | grep java | grep tomcat | grep -v 'grep' | awk '{print $2}')
    kill -9 $TPID
    sleep 5
    
    TSTAT=$(ps aux | grep java | grep tomcat | grep -v 'grep' | awk '{print $2}')
    if [ -z $TSTAT ];then
        echo "tomcat stop"
    else
        kill -9 $TSTAT
    fi
    cd $TOMCAT_PATH
    rm temp/* -rf
    rm work/* -rf
}

status () {
    ps aux | grep java | grep tomcat | grep -v 'grep' 
}


restart () {
    stop && start
}

main () {
case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    status)
        status
        ;;
    restart)
        restart
        ;;
    *)
        usage
        ;;
esac
}
main $1
