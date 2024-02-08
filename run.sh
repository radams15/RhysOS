#!/bin/sh

command="qemu-system-i386 -machine pc -fda build/system.img -boot a -m 1M -rtc base=localtime -vga cirrus -serial msmouse"
dbg_command="qemu-system-i386 -machine pc -fda build/system.img -boot a -m 1M -rtc base=localtime -vga cirrus -s -S"

if [ "$1" == "bochs" ]
then
    command="bochs -q"
elif [ "$1" == "debug" ]
then
    command=$dbg_command
elif [ "$1" == "build" ]
then
    command="echo"
fi

podman run -v .:/wdir:z -it --rm rhysos \
    && $command
