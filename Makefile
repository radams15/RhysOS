PROGRAMS=build/programs/test.bin

all: img

bload:
	@mkdir -p build
	nasm -fbin bootloader/boot.nasm -Ibootloader -o build/boot.bin
	
make_dir:
	gcc make_dir.c -o make_dir
	
load_file:
	gcc load_file.c -o load_file

krnl:
	bcc -ansi -c kernel/kernel.c -o build/kernel.o
	bcc -ansi -c kernel/fat.c -o build/fat.o
	bcc -ansi -c kernel/tty.c -o build/tty.o
	bcc -ansi -c kernel/util.c -o build/util.o
	
	nasm -fas86 kernel/interrupt.nasm -o build/interrupt.o
	nasm -fas86 kernel/proc.nasm -o build/proc.o
	
	ld86 -o build/kernel.bin -d build/kernel.o build/tty.o build/fat.o build/interrupt.o build/proc.o build/util.o

progs:
	@mkdir -p build/programs
	bcc -ansi -c programs/test.c -Iprograms/ -o build/programs/test.bin.o
	ld86 -o build/programs/test.bin -d build/programs/test.bin.o build/interrupt.o
	
	nasm -fbin programs/hello.nasm -Iprograms/ -o build/programs/hello.bin

img: bload krnl progs
	dd if=/dev/zero of=build/system.img bs=512 count=2880 # Empty disk
	
	dd if=build/boot.bin of=build/system.img bs=512 count=1 conv=notrunc
	
	dd if=build/kernel.bin of=build/system.img bs=512 seek=3 conv=notrunc
	
	dd if=map.img of=build/system.img bs=512 count=1 seek=1 conv=notrunc
	#dd if=dir.img of=build/system.img bs=512 count=1 seek=2 conv=notrunc
	
	./make_dir
	./load_file testfl
	cp build/programs/test.bin . && ./load_file test.bin && rm test.bin
	cp build/programs/hello.bin . && ./load_file hello.bin && rm hello.bin
	
run: img
	qemu-system-i386 -machine pc -fda build/system.img -boot a -m 1M
	
clean:
	rm -rf build
