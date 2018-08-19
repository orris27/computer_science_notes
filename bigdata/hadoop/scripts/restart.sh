#! /bin/sh

NODES="hadoop01 hadoop02 hadoop03"
HDFS_DIR=/hdfs
HADOOP_INSTALL=/usr/local/hadoop


# stop hadoop
${HADOOP_INSTALL}/sbin/stop-yarn.sh
${HADOOP_INSTALL}/sbin/stop-dfs.sh


# clean 
for i in ${NODES}
do
    ssh "$i" rm -rf ${HDFS_DIR}/*
    ssh "$i" /usr/local/hadoop/bin/hdfs namenode -format 
done


# start hadoop
${HADOOP_INSTALL}/sbin/start-yarn.sh
${HADOOP_INSTALL}/sbin/start-dfs.sh
