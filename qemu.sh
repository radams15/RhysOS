#!/bin/bash

/usr/bin/qemu-system-i386 -machine pc -fda build/system.img -boot a -m 10M -rtc base=localtime -vga cirrus
