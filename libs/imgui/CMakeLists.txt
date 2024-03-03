cmake_minimum_required(VERSION 3.11)
project(espert-external-libs-imgui)

set(CMAKE_CXX_STANDARD 20)

set(ESPERT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(INSTALL_DIR ${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR})

set(IMGUI_DIR ${ESPERT_DIR}/external/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imstb_rectpack.h
    ${IMGUI_DIR}/imstb_textedit.h
    ${IMGUI_DIR}/imstb_truetype.h
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.h
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)

set(IMGUI_INCLUDES
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imstb_rectpack.h
    ${IMGUI_DIR}/imstb_textedit.h
    ${IMGUI_DIR}/imstb_truetype.h
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.h
)

add_compile_definitions(IMGUI_IMPL_VULKAN_NO_PROTOTYPES)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PRIVATE ${ESPERT_DIR}/external/imgui)
target_include_directories(imgui PRIVATE ${ESPERT_DIR}/external/glfw/include)
target_include_directories(imgui PRIVATE ${ESPERT_DIR}/external/volk)
target_include_directories(imgui PUBLIC
    $<BUILD_INTERFACE:${IMGUI_INCLUDES}>
    $<INSTALL_INTERFACE:include>
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(FILES ${IMGUI_INCLUDES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set_target_properties(imgui PROPERTIES EXPORT_NAME "imgui")

install(TARGETS imgui EXPORT imguiConfig INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT imguiConfig DESTINATION "include/cmake/imgui")