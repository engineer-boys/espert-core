cmake_minimum_required(VERSION 3.16.0)

project(espert-core VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(ESPERT_DIR ${PROJECT_SOURCE_DIR})

option(ESP_BUILD_TESTS "Build the Espert-core test programs" OFF)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cc)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}/src
)

include_directories(${PROJECT_SOURCE_DIR})

target_precompile_headers(${PROJECT_NAME} PRIVATE src/esppch.hh)

############## Add libraries #######################
include(cmake/get-library.cmake)
get_library(TARGET catch2
    PATH lib/cmake/Catch2
    NAMES Catch2
)

get_library(TARGET entt
    PATH lib/EnTT/cmake
    NAMES EnTT
)

get_library(TARGET glfw
    PATH lib/cmake/glfw3
    NAMES glfw3
)

get_library(TARGET glm
    PATH lib/cmake/glm
    NAMES glm
)

get_library(TARGET glslang
    PATH lib/cmake/glslang
    NAMES glslang
)

get_library(TARGET spdlog
    PATH lib/cmake/spdlog
    NAMES spdlog
)

get_library(TARGET tinyobjloader
    PATH lib/tinyobjloader/cmake
    NAMES tinyobjloader
)

get_library(TARGET volk
    PATH lib/cmake/volk
    NAMES volk
)

if(NOT EXISTS ${ESPERT_DIR}/libs/vvl/${CMAKE_BUILD_TYPE})
    include(cmake/install-library.cmake)
    install_library(NAME vvl)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    volk::volk
    glfw
    glm::glm
    spdlog::spdlog_header_only
    tinyobjloader
    EnTT
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/stb)

############## Build shaders #######################
# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/resources/Shaders/*.frag"
        "${PROJECT_SOURCE_DIR}/resources/Shaders/*.vert"
        )

# run glslang compiler for every shader source file
foreach (GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    get_filename_component(DIR_PATH ${GLSL} DIRECTORY)
    get_filename_component(FILE_DIR ${DIR_PATH} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/resources/Shaders/${FILE_DIR}/${FILE_NAME}.spv")
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
