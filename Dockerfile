FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y software-properties-common make binutils perl libconfig-simple-perl libfile-find-rule-perl mtools dosfstools

RUN add-apt-repository -y ppa:tkchia/build-ia16 && apt-get update && apt-get install -y gcc-ia16-elf nasm

RUN apt-get install -y git

WORKDIR /wdir

ENTRYPOINT ["perl", "make.pl", "build"]
