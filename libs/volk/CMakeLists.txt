cmake_minimum_required(VERSION 3.11)
project(espert-external-libs-volk)

set(ESPERT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)

set(VOLK_INSTALL_DIR ${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_PREFIX ${VOLK_INSTALL_DIR})

set(VOLK_DIR "${ESPERT_DIR}/external/volk")
set(VOLK_FILES
    "${VOLK_DIR}/volk.c"
    "${VOLK_DIR}/volk.h")

add_library(volk STATIC ${VOLK_FILES})
add_library(volk_headers STATIC ${VOLK_FILES})
set_target_properties(volk PROPERTIES POSITION_INDEPENDENT_CODE ON)

include(${ESPERT_DIR}/cmake/get-library.cmake)
get_library(TARGET vulkan
    PATH include/cmake/vulkan
    NAMES Vulkan
)

target_link_libraries(volk PUBLIC Vulkan::Headers)

target_include_directories(volk PUBLIC 
    $<BUILD_INTERFACE:${VOLK_DIR}>
    $<INSTALL_INTERFACE:include>
)
if (VKB_WSI_SELECTION STREQUAL XCB)
    target_include_directories(volk PUBLIC ${XCB_INCLUDE_DIRS})
elseif (VKB_WSI_SELECTION STREQUAL XLIB)
    target_include_directories(volk PUBLIC ${X11_INCLUDE_DIRS})
elseif (VKB_WSI_SELECTION STREQUAL WAYLAND)
    target_include_directories(volk PUBLIC ${WAYLAND_INCLUDE_DIRS})
endif()

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${VOLK_INSTALL_DIR}/lib/cmake/volk)

install(FILES ${VOLK_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS volk volk_headers
  EXPORT volk-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT volk-targets
  FILE volkTargets.cmake
  NAMESPACE volk::
  DESTINATION ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)

configure_package_config_file(${VOLK_DIR}/cmake/volkConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfig.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)
