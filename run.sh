#!/bin/sh


podman run -v .:/wdir:z -it --rm rhysos \
    && bochs -q # qemu-system-i386 -machine pc -fda build/system.img -boot a -m 1M
