FROM ubuntu:18.04
RUN apt-get update \
  && apt-get install -y \
  curl \
  gcc \
  libc6-dev \
  qemu-system-arm \
  pixz \
  ssh \
  && curl -Lo dmd.deb http://downloads.dlang.org/releases/2.x/2.083.0/dmd_2.083.0-0_amd64.deb \
  && dpkg -i dmd.deb \
  && rm -v dmd.deb \
  && rm -rf /var/lib/apt/lists/*
