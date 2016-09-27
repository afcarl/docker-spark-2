FROM ubuntu:16.04

#USER root

#=====================================================================
#Step 1: Add basic linux tools and commands

#Based on: https://github.com/dockerfile/ubuntu/blob/master/Dockerfile

RUN \
  sed -i 's/# \(.*multiverse$\)/\1/g' /etc/apt/sources.list && \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get install -y build-essential && \
  apt-get install -y software-properties-common && \
  apt-get install -y byobu curl git htop man unzip vim wget && \
  rm -rf /var/lib/apt/lists/*

#=====================================================================
#Step 2: Install Oracle java

RUN apt-get install software-properties-common

#Based on https://github.com/dockerfile/java/blob/master/oracle-java8/Dockerfile
# Install Java.
RUN \
  echo oracle-java8-installer shared/accepted-oracle-license-v1-1 select true | debconf-set-selections && \
  apt-add-repository -y ppa:webupd8team/java && \
  apt-get update && \
  apt-get install -y oracle-java8-installer && \
  rm -rf /var/lib/apt/lists/* && \
  rm -rf /var/cache/oracle-jdk8-installer

# Define working directory.
WORKDIR /data

# Define commonly used JAVA_HOME variable
ENV JAVA_HOME /usr/lib/jvm/java-8-oracle

#=====================================================================
#Step 3: Download and untar Spark

RUN wget http://d3kbcqa49mib13.cloudfront.net/spark-2.0.0-bin-hadoop2.7.tgz

RUN tar -xzvf spark-2.0.0-bin-hadoop2.7.tgz

RUN echo "export PATH=\$PATH:~/spark-2.0.0-bin-hadoop2.7/bin" >> ~/.bashrc
RUN echo "export PATH=\$PATH:~/spark-2.0.0-bin-hadoop2.7/sbin" >> ~/.bashrc

#=======================================================================
#Step 4: Dowload and install Anaconda

#Based on: https://github.com/ContinuumIO/docker-images/blob/master/anaconda/Dockerfile
ENV LANG=C.UTF-8 LC_ALL=C.UTF-8

RUN apt-get update --fix-missing && apt-get install -y wget bzip2 ca-certificates \
    libglib2.0-0 libxext6 libsm6 libxrender1 \
    git mercurial subversion

RUN echo 'export PATH=/opt/conda/bin:$PATH' > /etc/profile.d/conda.sh && \
    wget --quiet https://repo.continuum.io/archive/Anaconda2-4.1.1-Linux-x86_64.sh -O ~/anaconda.sh && \
    /bin/bash ~/anaconda.sh -b -p /opt/conda && \
    rm ~/anaconda.sh

ENV PATH /opt/conda/bin:$PATH

#A nice looking name for jupyter
RUN conda create -y --name python27 python=2.7
RUN conda config --set core.default_env=python27

#RUN source activate python27 # to switch to python27

#
RUN ssh-keygen -q -N "" -t rsa -f /root/.ssh/id_rsa
RUN cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys

#As per: http://stackoverflow.com/questions/18136389/using-ssh-keys-inside-docker-container
RUN  echo "    IdentityFile ~/.ssh/id_rsa" >> /etc/ssh/ssh_config

#=======================================================================
#Step 3: Install tiny

#Based on recommendations in http://jupyter-notebook.readthedocs.io/en/latest/public_server.html

# Add Tini. Tini operates as a process subreaper for jupyter. This prevents
# kernel crashes.
ENV TINI_VERSION v0.10.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /usr/bin/tini
RUN chmod +x /usr/bin/tini
ENTRYPOINT ["/usr/bin/tini", "--"]


RUN apt-get install -y openssh-server
 
#Again spark

ENV SPARK_HOME /data/spark-2.0.0-bin-hadoop2.7

ENV PATH $SPARK_HOME/bin:$SPARK_HOME/sbin:$PATH


RUN sed  -i "/^[^#]*UsePAM/ s/.*/#&/"  /etc/ssh/sshd_config
RUN echo "UsePAM no" >> /etc/ssh/sshd_config
RUN echo "Port 2122" >> /etc/ssh/sshd_config

RUN ssh-keyscan -H localhost >> /root/.ssh/known_hosts


EXPOSE 8888 8080 8081 8082
EXPOSE 4040

ADD scripts /root/scripts
WORKDIR /root/scripts
CMD ["./run_spark_jupyter.sh"]

