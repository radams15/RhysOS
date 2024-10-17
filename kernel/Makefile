KERNEL_CFLAGS ?= -Wall -Werror -I. -DKERNEL_SEGMENT=${KERNEL_SEGMENT} -DRELEASE_VERSION=\"${tag}\" -DHEAP_ADDRESS=${HEAP_ADDR} -DKERNEL_SEGMENT=${KERNEL_SEGMENT}
KERNEL_CXXFLAGS ?= ${KERNEL_CFLAGS} -fno-unwind-tables -fno-rtti -fno-exceptions

KERNEL_SRCS := $(shell find * -name '*.cpp' -or -name '*.c' -or -name '*.nasm')
KERNEL_OBJS := $(KERNEL_SRCS:%=../${BUILD_DIR}/%.o)

all: $(KERNEL_OBJS)

../$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	${CC} -Ikernel $(KERNEL_CFLAGS) ${C_FLAGS} -c $< -o $@

../$(BUILD_DIR)/%.nasm.o: %.nasm
	mkdir -p $(dir $@)
	${ASM} -f elf -Ikernel $(KERNEL_CFLAGS) ${ASM_FLAGS} $< -o $@

.SUFFIXES: