export ASM = nasm
export CC = ia16-elf-gcc
export CXX = ia16-elf-g++
export LD = ia16-elf-ld

export BUILD_DIR ?= build/

export KERNEL_SEGMENT = 0x3000
export BOOT2_SEGMENT = 0x9000
export STACK_SEGMENT = ${KERNEL_SEGMENT}

export HEAP_ADDR = 0x9000
export FLOPPY_SECTORS = 2880

export ASM_FLAGS ?= -W-gnu-elf-extensions
export C_FLAGS ?= -fno-inline -ffreestanding -march=i8086 -mtune=i8086 -fleading-underscore
BOOTLOADER_CFLAGS ?= -Wall -Werror -Ibootloader -DKERNEL_SEGMENT=${KERNEL_SEGMENT} -DSTACK_SEG=${STACK_SEGMENT} -DBOOT2_SEG=${BOOT2_SEGMENT}
RUNTIME_CFLAGS ?= -DKERNEL_SEGMENT=${KERNEL_SEGMENT}

all: clean build_bootloader build_kernel build_runtime build_stdlib build_programs
	@echo "All done"

build_kernel:
	make -C kernel

build_stdlib:
	make -C stdlib

build_programs:
	make -C programs

clean:
	rm -rf build/
	@echo "Cleaned"

build_bootloader:
	mkdir -p ${BUILD_DIR}
	${ASM} bootloader/boot.nasm -f bin ${BOOTLOADER_CFLAGS} -o build/boot.bin

	${CC} ${BOOTLOADER_CFLAGS} ${C_FLAGS} -c bootloader/boot2.c -o build/boot2.o
	${ASM} ${BOOTLOADER_CFLAGS} ${ASM_FLAGS} -f elf bootloader/boot2.nasm -o build/boot2_asm.o
	${LD} -T bootloader/link.ld -o build/boot2.bin -d build/boot2.o build/boot2_asm.o

build_runtime:
	mkdir -p ${BUILD_DIR}
	${ASM} -felf runtime/crt0.nasm -Istdlib/real/ -o build/crt0_nasm.o ${RUNTIME_CFLAGS} -W-gnu-elf-extensions
	${ASM} -felf runtime/crt0.pmode.nasm -Istdlib/protected/ -o build/crt0_pmode_nasm.o ${RUNTIME_CFLAGS} -W-gnu-elf-extensions
	${CC} -c runtime/crt0.c -Istdlib/real -o build/crt0.o ${RUNTIME_CFLAGS} ${C_FLAGS}
	ar -rcs $(BUILD_DIR)/crt0.a build/crt0_nasm.o build/crt0.o
	ar -rcs $(BUILD_DIR)/crt0.pmode.a build/crt0_pmode_nasm.o