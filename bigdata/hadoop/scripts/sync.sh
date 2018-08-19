#! /bin/sh

WORKER_NODES="hadoop02 hadoop03"
HADOOP_INSTALL=/usr/local/hadoop
filename=$1

for i in ${WORKER_NODES}
do
    scp ${HADOOP_INSTALL}/etc/hadoop/"$filename" root@"$i":${HADOOP_INSTALL}/etc/hadoop/"$filename"
done
HDFS_DIR=/data
