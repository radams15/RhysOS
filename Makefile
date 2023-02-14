#FLOPPY_SIZE=720 # k
#ROOT_ENTRIES=112 # double for 1.44M floppy
#SECTORS=1440 # double for 1.44M floppy
#SECT_PER_FAT=3 # 9 for 1.44M floppy
#SECT_PER_TRACK=9 # double for 1.44 floppy
#SECT_PER_CLUSTER=2 # 1 for 1.44 floppy

FLOPPY_SIZE=1440 # k
ROOT_ENTRIES=224 # double for 1.44M floppy
SECTORS=2880 # double for 1.44M floppy
SECT_PER_FAT=9 # 9 for 1.44M floppy
SECT_PER_TRACK=18 # double for 1.44 floppy
SECT_PER_CLUSTER=1 # 1 for 1.44 floppy

PROGRAMS=build/programs/hello.bin

all: img

bload:
	nasm -fbin \
    	-D SECTORS=${SECTORS} \
    	-D ROOT_ENTRIES=${ROOT_ENTRIES} \
    	-D SECT_PER_FAT=${SECT_PER_FAT} \
    	-D SECT_PER_TRACK=${SECT_PER_TRACK} \
    	-D SECT_PER_CLUSTER=${SECT_PER_CLUSTER} \
    	bootloader/boot.nasm -Ibootloader -o build/boot.bin

krnl:
	bcc -ansi -Ms -0 -S kernel/kernel.c -o build/kernel.s
	sed -i 's/\.data//g' build/kernel.s
	sed -i 's/\.text//g' build/kernel.s
	as86 -2 -w- -b build/kernel.bin build/kernel.s
	
	@#as86 -2 -o build/kernel.o build/kernel.s
	@#ld86 -o build/kernel.elf -T0x2000 build/kernel.o 

build/programs/%.bin: programs/%.nasm
	@mkdir -p build/programs
	nasm -fbin $< -Iprograms/ -o $@

img: bload krnl template.img ${PROGRAMS}
	cp template.img build/boot.img
	dd status=noxfer conv=notrunc if=build/boot.bin of=build/boot.img
	
	rm -rf .tmp/
	mkdir -p .tmp/loop/
	sudo mount -o loop -t vfat build/boot.img .tmp/loop
	sudo cp build/kernel.bin .tmp/loop
	sudo cp build/programs/* .tmp/loop
	sudo umount .tmp/loop
	
run: img
	qemu-system-i386 -machine pc -fda build/boot.img -boot a -m 1M
	
template.img:
	mkdosfs -C template.img ${FLOPPY_SIZE}
