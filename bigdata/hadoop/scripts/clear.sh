#! /bin/sh

NODES="hadoop01 hadoop02 hadoop03"
HDFS_DIR=/data

for i in ${NODES}
do
    ssh "$i" rm -rf ${HDFS_DIR}/*
    ssh "$i" /usr/local/hadoop/bin/hdfs namenode -format 
done
