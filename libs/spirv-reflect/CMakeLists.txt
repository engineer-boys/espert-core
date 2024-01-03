cmake_minimum_required(VERSION 3.11)
project(espert-external-libs-spirv-reflect)

set(ESPERT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(INSTALL_DIR ${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR})


set(SPIRV_REFLECT_DIR ${ESPERT_DIR}/external/spirv-reflect)
set(SPIRV_REFLECT_INCLUDES ${SPIRV_REFLECT_DIR}/spirv_reflect.h ${SPIRV_REFLECT_DIR}/include)

add_library(spirv-reflect STATIC
    ${SPIRV_REFLECT_DIR}/spirv_reflect.h
    ${SPIRV_REFLECT_DIR}/spirv_reflect.c
)

target_include_directories(spirv-reflect INTERFACE
    $<BUILD_INTERFACE:${SPIRV_REFLECT_INCLUDES}>
    $<INSTALL_INTERFACE:include>
)


include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


set(INSTALL_CONFIGDIR ${INSTALL_DIR}/lib/cmake/spirv-reflect)

install(FILES ${SPIRV_REFLECT_DIR}/spirv_reflect.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${SPIRV_REFLECT_DIR}/include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS spirv-reflect
  EXPORT spirv-reflect-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT spirv-reflect-targets
  FILE spirv-reflectTargets.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/spirv-reflectConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/spirv-reflectConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/spirv-reflectConfig.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)
