STDLIB_CFLAGS ?=
STDLIB_CXXFLAGS ?= ${STDLIB_CFLAGS} -fno-unwind-tables -fno-rtti -fno-exceptions

STDLIB_SRCS := $(shell find real -name '*.cpp' -or -name '*.c' -or -name '*.nasm')
STDLIB_OBJS := $(STDLIB_SRCS:%=../${BUILD_DIR}/stdlib/%.o)

all: folders $(STDLIB_OBJS)
	ar -rcs ../$(BUILD_DIR)/stdlib/real/libstdlib.a $(STDLIB_OBJS)

folders:
	mkdir -p ../$(BUILD_DIR)/stdlib/real

../$(BUILD_DIR)/stdlib/%.c.o: %.c
	mkdir -p $(dir $@)
	${CC} -Ireal ${STDLIB_CFLAGS} ${C_FLAGS} -c $< -o $@

../$(BUILD_DIR)/stdlib/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	${CXX} -Ireal ${STDLIB_CXXFLAGS} ${C_FLAGS} -c $< -o $@

../$(BUILD_DIR)/stdlib/%.nasm.o: %.nasm
	mkdir -p $(dir $@)
	${ASM} -f elf -Ireal ${ASM_FLAGS} $< -o $@