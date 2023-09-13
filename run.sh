#!/bin/sh

command="qemu-system-i386 -machine pc -fda build/system.img -boot a -m 1M"

if [ "$1" == "bochs" ]
then
    command="bochs -q"
fi

podman run -v .:/wdir:z -it --rm rhysos \
    && $command
