FROM ubuntu:18.10
RUN apt-get update && apt-get install -y \
  ccache \
  curl \
  gcc \
  git \
  ninja-build \
  pigz \
  pixz \
  sshpass \
  wget \
  xz-utils \
  ffmpeg \
  && rm -rfv /var/lib/apt/lists/*
RUN  curl -Lo dmd.deb http://downloads.dlang.org/releases/2.x/2.084.0/dmd_2.084.0-0_amd64.deb \
  && dpkg -i dmd.deb \
  && rm -v dmd.deb
RUN mkdir cmake \
  && cd cmake \
  && curl -Lo cmake.tar.gz https://github.com/Kitware/CMake/releases/download/v3.14.1/cmake-3.14.1-Linux-x86_64.tar.gz \
  && tar --strip-components=1 -xf cmake.tar.gz \
  && rm -v cmake.tar.gz
ENV PATH="$PWD/cmake/bin:$PATH"
