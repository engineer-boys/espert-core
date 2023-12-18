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

if (ESP_BUILD_TESTS)
    get_library(NAME catch2
        CONF_PATH lib/cmake/Catch2
        CONF_NAMES Catch2
    )
endif()

get_library(NAME entt
    CONF_PATH lib/EnTT/cmake
    CONF_NAMES EnTT
)

get_library(NAME glfw
    CONF_PATH lib/cmake/glfw3
    CONF_NAMES glfw3
)

get_library(NAME glm
    CONF_PATH lib/cmake/glm
)

get_library(NAME glslang
    CONF_PATH lib/cmake/glslang
)

get_library(NAME spdlog
    CONF_PATH lib/cmake/spdlog
)

get_library(NAME stb
    CONF_PATH include/cmake/stb
)

get_library(NAME assimp
    CONF_PATH lib/cmake/assimp-5.3)

include(${ESPERT_DIR}/cmake/get-vulkan.cmake)
get_vulkan()

if(ESP_BUILD_VVL AND NOT WIN32 AND NOT EXISTS ${ESPERT_DIR}/libs/vvl/${CMAKE_BUILD_TYPE})
    include(cmake/install-library.cmake)
    install_library(NAME vvl
        PREFIX ${ESPERT_DIR}/libs/vvl/${CMAKE_BUILD_TYPE}
    )
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND python3 scripts/post-build.py --${BUILD_TYPE_LOWER} ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Running post-build script..."
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    volk
    glfw
    glm::glm
    spdlog::spdlog_header_only
    EnTT::EnTT
    stb
    assimp::assimp
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
add_dependencies(core_shaders glslang::glslang-standalone)

############## Build tests #########################
if (ESP_BUILD_TESTS)
    file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/tests/*.cc)
    add_executable(tests ${TEST_SOURCES})
    target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR}/tests)
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
    target_link_libraries(tests PUBLIC espert-core)

    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/external/catch2/extras)
    include(CTest)
    include(Catch)
    catch_discover_tests(tests)
endif()
