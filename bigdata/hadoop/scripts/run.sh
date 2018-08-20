#! /bin/sh

javac -classpath $(hadoop classpath) *.java && jar cvf ./JobMain.jar ./*.class && hadoop jar JobMain.jar JobMain /data/sample.txt /data/out
