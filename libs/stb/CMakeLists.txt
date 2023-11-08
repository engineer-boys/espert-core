cmake_minimum_required(VERSION 3.11)
project(espert-external-libs-stb-image)

set(ESPERT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(INSTALL_DIR ${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR})

add_library(stb INTERFACE)
set(STB_DIR ${ESPERT_DIR}/external/stb)
target_sources(stb INTERFACE
    $<BUILD_INTERFACE:${STB_DIR}/stb_image.h>
    $<INSTALL_INTERFACE:include>
)
target_include_directories(stb INTERFACE
    $<BUILD_INTERFACE:${STB_DIR}>
    $<INSTALL_INTERFACE:include>
)


include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY "${STB_DIR}" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set_target_properties(stb PROPERTIES EXPORT_NAME "stb")

install(TARGETS stb EXPORT stbConfig INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT stbConfig DESTINATION "include/cmake/stb")