ASM = nasm
CC = ia16-elf-gcc
CXX = ia16-elf-g++
LD = ia16-elf-ld

KERNEL_SEGMENT = 0x3000
BOOT2_SEGMENT = 0x9000
STACK_SEGMENT = ${KERNEL_SEGMENT}

HEAP_ADDR = 0x9000
FLOPPY_SECTORS = 2880

ASM_FLAGS ?= -W-gnu-elf-extensions
C_FLAGS ?= -fno-inline -ffreestanding -march=i8086 -mtune=i8086 -fleading-underscore
BOOTLOADER_CFLAGS ?= -Wall -Werror -Ibootloader -DKERNEL_SEGMENT=${KERNEL_SEGMENT} -DSTACK_SEG=${STACK_SEGMENT} -DBOOT2_SEG=${BOOT2_SEGMENT}
KERNEL_CFLAGS ?= -Wall -Werror -DRELEASE_VERSION=\"${tag}\" -DHEAP_ADDRESS=${HEAP_ADDR} -DKERNEL_SEGMENT=${KERNEL_SEGMENT}
KERNEL_CXXFLAGS ?= ${KERNEL_CFLAGS} -fno-unwind-tables -fno-rtti -fno-exceptions
PROGRAM_CFLAGS ?= -Wall -DKERNEL_SEGMENT=${KERNEL_SEGMENT}

BOOT1_SRC = bootloader/boot.nasm

all: clean build_bootloader
	gcc --help
	@echo "All done"

clean:
	rm -rf build/
	@echo "Cleaned"

build_bootloader:
	mkdir -p build
	${ASM} ${BOOT1_SRC} -f bin ${BOOTLOADER_CFLAGS} -o build/boot.bin

	${CC} ${BOOTLOADER_CFLAGS} ${C_FLAGS} -c bootloader/boot2.c -o build/boot2.o
	${ASM} ${BOOTLOADER_CFLAGS} ${ASM_FLAGS} -f elf bootloader/boot2.nasm -o build/boot2_asm.o
	${LD} -T bootloader/link.ld -o build/boot2.bin -d build/boot2.o build/boot2_asm.o