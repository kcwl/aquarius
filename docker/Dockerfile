FROM moongeun/centos9:latest

RUN yum install gcc-toolset-14 && \
    scl enable gcc-toolset-14 bash

RUN yum install -y openssl openssl-devel wget python git cmake

RUN wget https://archives.boost.io/release/1.90.0/source/boost_1_90_0.tar.gz &&\
    tar zxvf boost_1_90_0.tar.gz && \
    cd boost_1_90_0 && \
    ./bootstrap.sh && \
    ./b2 && \
    ./b2 install

RUN wget https://dev.mysql.com/get/mysql84-community-release-el9-2.noarch.rpm && \
    rpm -ivh mysql84-community-release-el9-2.noarch.rpm && \
    yum install -y mysql-community-server mysql-community-client mysql-community-common mysql-community-devel