#!/bin/bash

# use environment python27 (python 2.7) from anaconda
source activate python27

# start ssh deamon
/etc/init.d/ssh start

#start spark master and worker
$SPARK_HOME/sbin/start-all.sh

#start jupyter with spark in Spark standalone mode 
PYSPARK_DRIVER_PYTHON=jupyter \
PYSPARK_PYTHON=/opt/conda/envs/python27/bin/python \
PYSPARK_DRIVER_PYTHON_OPTS="notebook --no-browser --port=8888 --ip='0.0.0.0'" \
pyspark --master spark://`hostname`:7077

#PYSPARK_PYTHON=jupyter \
#PYSPARK_PYTHON_OPTS="notebook --no-browser --port=8888 --ip='0.0.0.0'" \
# pyspark --master spark://`hostname`:7077

