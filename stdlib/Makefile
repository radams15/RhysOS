STDLIB_CFLAGS ?=
STDLIB_CXXFLAGS ?= ${STDLIB_CFLAGS} -fno-unwind-tables -fno-rtti -fno-exceptions

STDLIB_SRCS := $(shell find stdlib/real -name '*.cpp' -or -name '*.c' -or -name '*.nasm')
STDLIB_OBJS := $(STDLIB_SRCS:%=${BUILD_DIR}/%.o)

stdlib: $(STDLIB_OBJS)
	ar -rcs $(BUILD_DIR)/stdlib/real/libstdlib.a $(STDLIB_OBJS)

$(BUILD_DIR)/%.c.o: %.c forcestdlib
	mkdir -p $(dir $@)
	${CC} -Istdlib/real ${STDLIB_CFLAGS} ${C_FLAGS} -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp forcestdlib
	mkdir -p $(dir $@)
	${CXX} -Istdlib/real ${STDLIB_CXXFLAGS} ${C_FLAGS} -c $< -o $@

$(BUILD_DIR)/%.nasm.o: %.nasm forcestdlib
	mkdir -p $(dir $@)
	${ASM} -f elf -Istdlib/real ${ASM_FLAGS} $< -o $@

forcestdlib: ;