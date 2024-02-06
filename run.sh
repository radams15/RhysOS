#!/bin/sh

command="qemu-system-i386 -machine pc -fda build/system.img -boot a -m 1M -rtc base=localtime -vga cirrus -serial msmouse"

if [ "$1" == "bochs" ]
then
    command="bochs -q"
elif [ "$1" == "build" ]
then
    command="echo"
fi

podman run -v .:/wdir:z -it --rm rhysos \
    && $command
