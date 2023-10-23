cmake_minimum_required(VERSION 3.16.0)

project(espert-core VERSION 0.0.1 LANGUAGES C CXX)

set (CMAKE_CXX_STANDARD 20)

option(ESP_BUILD_TESTS "Build the Espert-core test programs" OFF)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cc)

# Remove uncessary files
if(RENDER_PLATFORM STREQUAL VULKAN)
    file(GLOB_RECURSE BAD_SOURCES ${PROJECT_SOURCE_DIR}/src/Platforms/OpenGLMiniRAPI\*.cc)
    foreach(bad_file ${BAD_SOURCES})
        list(REMOVE_ITEM SOURCES ${bad_file})
    endforeach()
elseif(RENDER_PLATFORM STREQUAL OPENGL)
    file(GLOB_RECURSE BAD_SOURCES ${PROJECT_SOURCE_DIR}/src/Platforms/VulkanMiniRAPI\*.cc)
    foreach(bad_file ${BAD_SOURCES})
        list(REMOVE_ITEM SOURCES ${bad_file})
    endforeach()
endif()

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
)

# Set render platform specific macro
if(RENDER_PLATFORM STREQUAL VULKAN)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VULKAN_PLATFORM)
elseif(RENDER_PLATFORM STREQUAL OPENGL)
        target_compile_definitions(${PROJECT_NAME} PRIVATE OPENGL_PLATFORM)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}/src
)

include_directories(${PROJECT_SOURCE_DIR})

target_precompile_headers(${PROJECT_NAME} PRIVATE src/esppch.hh)

############## Add libraries #######################
add_subdirectory(external)

target_link_libraries(${PROJECT_NAME} PUBLIC
    volk
    glfw
    glm
    spdlog_header_only
)

############## Build shaders #######################
# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)

# run glslang compiler for every shader source file
foreach (GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    get_filename_component(DIR_PATH ${GLSL} DIRECTORY)
    get_filename_component(FILE_DIR ${DIR_PATH} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND $<TARGET_FILE:glslang-standalone> -V ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach (GLSL)

add_custom_target(
    core_shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)
add_dependencies(${PROJECT_NAME} core_shaders)

############## Build tests #########################
if (ESP_BUILD_TESTS)
    file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/tests/*.cc)
    add_executable(tests ${TEST_SOURCES})
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/external/catch2/extras)
    include(CTest)
    include(Catch)
    catch_discover_tests(tests)
endif()
