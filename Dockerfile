FROM debian:stable

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y bcc nasm make binutils perl libconfig-simple-perl libfile-find-rule-perl

WORKDIR /wdir

ENTRYPOINT ["perl", "make.pl", "build"]
