bload:
	nasm -fbin bootloader/boot.nasm -Ibootloader -o build/boot.bin

bak_krnl:
	nasm -felf kernel/kernel.nasm -Ikernel -o build/kernel.o
	i386-elf-objcopy -O binary -o build/kernel.bin build/kernel.o 

krnl:
	bcc -ansi -0 -S kernel/kernel.c -o build/kernel.s
	sed -i 's/\.data//g' build/kernel.s
	sed -i 's/\.text//g' build/kernel.s
	as86 -b build/kernel.bin build/kernel.s
	#as86 -b build/kernel.bin kernel/kernel.fasm

img: bload krnl
	cat build/boot.bin build/kernel.bin > build/boot.img
