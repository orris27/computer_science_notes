#! /bin/sh

NODES="hadoop01 hadoop02 hadoop03"
HDFS_DIR=/hdfs
HADOOP_INSTALL=/usr/local/hadoop

for i in ${NODES}
do
    ssh "$i" rm -rf ${HDFS_DIR}/*
    ssh "$i" ${HADOOP_INSTALL}/bin/hdfs namenode -format 
done
