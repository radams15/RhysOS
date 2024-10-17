PROGRAM_FLAGS ?= -Wall -DKERNEL_SEGMENT=$KERNEL_SEGMENT

PROGRAM_DIRS := $(shell echo */)
PROGRAM_BINS := $(PROGRAM_DIRS:%=../${BUILD_DIR}/%)

MKPROG?=./mkprog.pl

all: make_folder $(PROGRAM_BINS)
	@echo $(PROGRAM_BINS)

make_folder:
	mkdir -p ../$(BUILD_DIR)/programs

../$(BUILD_DIR)/%:%
	CC=${CC} CXX=${CXX} LD=${LD} ASM=${ASM} PROGRAM_FLAGS="${C_FLAGS} ${PROGRAM_FLAGS}" \
 		${MKPROG} $< $@