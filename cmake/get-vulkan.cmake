macro(get_vulkan)
    set(vulkan_DIR ${ESPERT_DIR}/libs/volk/${CMAKE_BUILD_TYPE}/include/cmake/vulkan)
    find_package(vulkan
        NAMES vulkan
        NO_DEFAULT_PATH
    )
    set(volk_DIR ${ESPERT_DIR}/libs/volk/${CMAKE_BUILD_TYPE}/lib/cmake/volk)
    find_package(volk
        NAMES volk
        NO_DEFAULT_PATH
    )

    if(NOT ${vulkan_FOUND} OR NOT ${volk_FOUND})
        include(${ESPERT_DIR}/cmake/install-library.cmake)
        install_library(NAME volk
            PREFIX ${ESPERT_DIR}/libs/volk/${CMAKE_BUILD_TYPE}
        )

        set(vulkan_DIR ${ESPERT_DIR}/libs/volk/${CMAKE_BUILD_TYPE}/include/cmake/vulkan)
        find_package(vulkan
            NAMES vulkan
            NO_DEFAULT_PATH
            REQUIRED
        )
        set(volk_DIR ${ESPERT_DIR}/libs/volk/${CMAKE_BUILD_TYPE}/lib/cmake/volk)
        find_package(volk
            NAMES volk
            NO_DEFAULT_PATH
            REQUIRED
        )
    endif()

endmacro()
