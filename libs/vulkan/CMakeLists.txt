set(INSTALL_DIR ${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR})

set(ESPERT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(VULKAN_INCLUDE_DIR ${ESPERT_DIR}/external/vulkan/include)

add_library(vulkan INTERFACE)

target_sources(vulkan INTERFACE
    $<BUILD_INTERFACE:${VULKAN_INCLUDE_DIR}/vulkan/vulkan.h>
    $<INSTALL_INTERFACE:include/vulkan>
)
target_include_directories(vulkan INTERFACE
    $<BUILD_INTERFACE:${VULKAN_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/vulkan>
)

target_compile_definitions(vulkan INTERFACE VK_NO_PROTOTYPES)

if(WIN32)
    target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_WIN32_KHR)
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

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY "${VULKAN_INCLUDE_DIR}/vk_video" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY "${VULKAN_INCLUDE_DIR}/vulkan" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set_target_properties(vulkan PROPERTIES EXPORT_NAME "Headers")

install(TARGETS vulkan EXPORT VulkanConfig INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT VulkanConfig NAMESPACE "Vulkan::" DESTINATION "include/cmake/vulkan")