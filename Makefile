
SRC_DIR := src
CORE_SYSTEM_DIR := CoreSystems
ARFLAGS := rcs
BUILD := build
EXTERNALS := externals

EXTERNALS_SPDLOG_HEADER := ${EXTERNALS}/spdlog/include

EXTERNALS_GLFW3_HEADER := ${EXTERNALS}/glfw/include
EXTERNALS_GLFW3_LIB := ${EXTERNALS}/glfw/build/src/libglfw3.a

CORES := ${SRC_DIR}/${CORE_SYSTEM_DIR}

.PHONY: libesper, clean

libesper: \
  ${BUILD}/Application.o \
  ${BUILD}/Logger.o \
  ${BUILD}/ContextApp.o \
  ${BUILD}/LayerStack.o \
  ${BUILD}/EspertWindow.o
	$(AR) $(ARFLAGS) ${BUILD}/$@.a $^

${BUILD}/ContextApp.o: ${CORES}/ContextApplication.cc
	g++ -c $< -o $@ -I${EXTERNALS_SPDLOG_HEADER}

${BUILD}/Application.o: ${CORES}/Application.cc
	g++ -c $< -o $@ -I${EXTERNALS_SPDLOG_HEADER} -I${EXTERNALS_GLFW3_HEADER}

${BUILD}/Logger.o: ${CORES}/Logger.cc
	g++ -c $< -o $@ -I${EXTERNALS_SPDLOG_HEADER}

${BUILD}/LayerStack.o: ${CORES}/Layers/LayerStack.cc
	g++ -c $< -o $@

${BUILD}/EspertWindow.o: ${CORES}/EspertWindow.cc
	g++ -c $< -o $@ -I${EXTERNALS_GLFW3_HEADER} ${EXTERNALS_GLFW3_LIB} -I${EXTERNALS_SPDLOG_HEADER}

clean:
	rm -rf build/*
