cmake_minimum_required(VERSION 3.11)
project(espert-external-libs-volk)

set(ESPERT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(INSTALL_DIR ${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR})


set(VULKAN_INCLUDE_DIR ${ESPERT_DIR}/external/vulkan/include)

add_library(vulkan INTERFACE)

target_sources(vulkan INTERFACE
    $<BUILD_INTERFACE:${VULKAN_INCLUDE_DIR}/vulkan/vulkan.h>
    $<INSTALL_INTERFACE:include>
)
target_include_directories(vulkan INTERFACE
    $<BUILD_INTERFACE:${VULKAN_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)


set(VOLK_DIR "${ESPERT_DIR}/external/volk")
set(VOLK_FILES
    "${VOLK_DIR}/volk.c"
    "${VOLK_DIR}/volk.h")

add_library(volk STATIC ${VOLK_FILES})
add_library(volk_headers INTERFACE)
set_target_properties(volk PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(volk INTERFACE vulkan)

set(VOLK_INCLUDES
    "${ESPERT_DIR}/external/volk"
    "${ESPERT_DIR}/external/vulkan/include"
)

target_include_directories(volk PUBLIC 
    $<BUILD_INTERFACE:${VOLK_INCLUDES}>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(vulkan INTERFACE VK_NO_PROTOTYPES)

if(WIN32)
    target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_WIN32_KHR)
    target_compile_definitions(volk PUBLIC VK_USE_PLATFORM_WIN32_KHR)
elseif(APPLE)
	target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_METAL_EXT)
elseif(UNIX)
    # Choose WSI based on VKB_WSI_SELECTION
    if (VKB_WSI_SELECTION STREQUAL XCB OR VKB_WSI_SELECTION STREQUAL XLIB OR VKB_WSI_SELECTION STREQUAL WAYLAND)
        find_package(PkgConfig REQUIRED)
    endif()
    if (VKB_WSI_SELECTION STREQUAL XCB)
        pkg_check_modules(XCB xcb REQUIRED)
        if (XCB_FOUND)
            target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_XCB_KHR)
        endif()
    elseif (VKB_WSI_SELECTION STREQUAL XLIB)
        pkg_check_modules(X11 x11 REQUIRED)
        if (X11_FOUND)
            target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_XLIB_KHR)
        endif()
    elseif (VKB_WSI_SELECTION STREQUAL WAYLAND)
        pkg_check_modules(WAYLAND wayland-client REQUIRED)
        if (WAYLAND_FOUND)
            target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_WAYLAND_KHR)
        endif()
    elseif (VKB_WSI_SELECTION STREQUAL D2D)
        set(DIRECT_TO_DISPLAY TRUE)
        set(DIRECT_TO_DISPLAY TRUE PARENT_SCOPE)
        target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_DISPLAY_KHR)
    else()
        message(FATAL_ERROR "Unknown WSI")
    endif()
endif()

if (VKB_WSI_SELECTION STREQUAL XCB)
    target_include_directories(volk PUBLIC ${XCB_INCLUDE_DIRS})
elseif (VKB_WSI_SELECTION STREQUAL XLIB)
    target_include_directories(volk PUBLIC ${X11_INCLUDE_DIRS})
elseif (VKB_WSI_SELECTION STREQUAL WAYLAND)
    target_include_directories(volk PUBLIC ${WAYLAND_INCLUDE_DIRS})
endif()


include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


install(DIRECTORY "${VULKAN_INCLUDE_DIR}/vk_video" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY "${VULKAN_INCLUDE_DIR}/vulkan" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set_target_properties(vulkan PROPERTIES EXPORT_NAME "vulkan")

install(TARGETS vulkan EXPORT vulkanConfig INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT vulkanConfig DESTINATION "include/cmake/vulkan")


set(INSTALL_CONFIGDIR ${INSTALL_DIR}/lib/cmake/volk)

install(FILES ${VOLK_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS volk volk_headers
  EXPORT volk-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT volk-targets
  FILE volkTargets.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/volkConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/volkConfig.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)
