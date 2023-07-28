FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y software-properties-common make binutils perl libconfig-simple-perl libfile-find-rule-perl

RUN add-apt-repository -y ppa:tkchia/build-ia16 && apt-get install -y gcc-ia16-elf nasm

RUN apt-get install -y mtools dosfstools

WORKDIR /wdir

ENTRYPOINT ["perl", "make.pl", "build"]
