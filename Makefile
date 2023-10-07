
SRC_DIR := src
CORE_SYSTEM_DIR := CoreSystems
ARFLAGS := rcs
BUILD := build

CORES := ${SRC_DIR}/${CORE_SYSTEM_DIR}

.PHONY: libespert

libespert: ${BUILD}/Application.o
	$(AR) $(ARFLAGS) ${BUILD}/$@.a $^

${BUILD}/Application.o: ${CORES}/Application.cc
	g++ -c $< -o $@
