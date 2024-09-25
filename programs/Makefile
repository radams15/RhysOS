PROGRAM_FLAGS ?= -Wall -DKERNEL_SEGMENT=$KERNEL_SEGMENT

PROGRAM_DIRS := $(shell echo programs/*/)
PROGRAM_BINS := $(PROGRAM_DIRS:%=${BUILD_DIR}/%)

MKPROG?=programs/mkprog.pl

programs: make_folder $(PROGRAM_BINS)

make_folder:
	mkdir -p $(BUILD_DIR)/programs

$(BUILD_DIR)/%:%
	CC=${CC} CXX=${CXX} LD=${LD} ASM=${ASM} PROGRAM_FLAGS="${C_FLAGS} ${PROGRAM_FLAGS}" \
 		${MKPROG} $< $@