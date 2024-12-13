FROM ubuntu:16.04

MAINTAINER Markus Lanner
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
  git wget pixz pbzip2 g++ mountall xz-utils unzip \
  && rm -rfv /var/lib/apt/lists/*
RUN mkdir -p /opt/build_tools \
  && git clone --depth=1 --branch=master https://github.com/raspberrypi/tools.git /opt/build_tools \
  && rm -rf build_tools/.git
# COPY 2016_03_01_v0_16_0.img /opt/base_image/
#COPY build_qt /opt/qt/
#RUN mkdir -p /opt/base_image \
# && cd /opt/base_image \
# && wget http://linkbit.io/software/hsb/base.img.xz -O base.img.xz \
# && xz -d base.img.xz
RUN mkdir -p /opt/qt \
 && wget http://download.qt.io/official_releases/qt/5.7/5.7.0/single/qt-everywhere-opensource-src-5.7.0.tar.gz -O /opt/qt/qt.tar.gz
RUN mkdir -p /opt/boost \
  && wget "http://downloads.sourceforge.net/project/boost/boost/1.61.0/boost_1_61_0.tar.bz2?r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F1.61.0%2F&ts=1467968054&use_mirror=heanet" -O /opt/boost/boost.tar.bz2 \
  && cd /opt/boost \
  && pbzip2 -d boost.tar.bz2
# TODO add websocketpp, enocean
COPY build_all build_boost /opt/
COPY site-config.jam /etc/
COPY base.img.zip /opt/base_image/base.img.zip
RUN cd /opt/base_image/ \
  && unzip base.img.zip \
  && mv -v *raspbian* base.img \
  && rm -v base.img.zip
ENTRYPOINT /opt/build_all
# && cd /opt/base_image/ \
# && pixz -d base.img.xz \
# && ls -lisah \
# && mount -o loop,offset=272629760 /opt/base_image/2016_03_01_v0_16_0.img /mnt
#RUN /opt/qt/build_qt



