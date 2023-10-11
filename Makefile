
SRC_DIR := src
CORE_SYSTEM_DIR := CoreSystems
ARFLAGS := rcs
BUILD := build
EXTERNALS := externals

EXTERNALS_SPDLOG_HEADER := ${EXTERNALS}/spdlog/include

CORES := ${SRC_DIR}/${CORE_SYSTEM_DIR}

.PHONY: libesper, clean

libesper: \
  ${BUILD}/Application.o \
  ${BUILD}/Logger.o \
  ${BUILD}/ContextApp.o \
  ${BUILD}/LayerStack.o
	$(AR) $(ARFLAGS) ${BUILD}/$@.a $^

${BUILD}/ContextApp.o: ${CORES}/ContextApplication.cc
	g++ -c $< -o $@ -I${EXTERNALS_SPDLOG_HEADER}

${BUILD}/Application.o: ${CORES}/Application.cc
	g++ -c $< -o $@ -I${EXTERNALS_SPDLOG_HEADER}

${BUILD}/Logger.o: ${CORES}/Logger.cc
	g++ -c $< -o $@ -I${EXTERNALS_SPDLOG_HEADER}

${BUILD}/LayerStack.o: ${CORES}/Layers/LayerStack.cc
	g++ -c $< -o $@

clean:
	rm -rf build/*
