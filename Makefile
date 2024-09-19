ASM = nasm
CC = ia16-elf-gcc
CXX = ia16-elf-g++
LD = ia16-elf-ld

BUILD_DIR ?= build/

KERNEL_SEGMENT = 0x3000
BOOT2_SEGMENT = 0x9000
STACK_SEGMENT = ${KERNEL_SEGMENT}

HEAP_ADDR = 0x9000
FLOPPY_SECTORS = 2880

ASM_FLAGS ?= -W-gnu-elf-extensions
C_FLAGS ?= -fno-inline -ffreestanding -march=i8086 -mtune=i8086 -fleading-underscore
BOOTLOADER_CFLAGS ?= -Wall -Werror -Ibootloader -DKERNEL_SEGMENT=${KERNEL_SEGMENT} -DSTACK_SEG=${STACK_SEGMENT} -DBOOT2_SEG=${BOOT2_SEGMENT}
PROGRAM_CFLAGS ?= -Wall -DKERNEL_SEGMENT=${KERNEL_SEGMENT}

include kernel/Makefile
include stdlib/Makefile

all: clean build_bootloader $(KERNEL_OBJS)
	@echo "All done"

clean:
	rm -rf build/
	@echo "Cleaned"

build_bootloader:
	mkdir -p ${BUILD_DIR}
	${ASM} bootloader/boot.nasm -f bin ${BOOTLOADER_CFLAGS} -o build/boot.bin

	${CC} ${BOOTLOADER_CFLAGS} ${C_FLAGS} -c bootloader/boot2.c -o build/boot2.o
	${ASM} ${BOOTLOADER_CFLAGS} ${ASM_FLAGS} -f elf bootloader/boot2.nasm -o build/boot2_asm.o
	${LD} -T bootloader/link.ld -o build/boot2.bin -d build/boot2.o build/boot2_asm.o