macro(get_library)
    set(oneValueArgs NAME CONF_PATH LIB_DIR_NAME)
    set(multiValueArgs CONF_NAMES)
    cmake_parse_arguments(GET_LIBRARY "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT GET_LIBRARY_CONF_NAMES)
        set(GET_LIBRARY_CONF_NAMES ${GET_LIBRARY_NAME})
    endif()

    if (NOT GET_LIBRARY_LIB_DIR_NAME)
        set(GET_LIBRARY_LIB_DIR_NAME ${GET_LIBRARY_NAME})
    endif()

    set(${GET_LIBRARY_NAME}_DIR ${ESPERT_DIR}/libs/${GET_LIBRARY_LIB_DIR_NAME}/${CMAKE_BUILD_TYPE}/${GET_LIBRARY_CONF_PATH})
    find_package(${GET_LIBRARY_NAME} 
        NAMES ${GET_LIBRARY_CONF_NAMES}
        NO_DEFAULT_PATH
    )

    if(NOT ${${GET_LIBRARY_NAME}_FOUND})
        include(${ESPERT_DIR}/cmake/install-library.cmake)
        install_library(NAME ${GET_LIBRARY_NAME}
            PREFIX ${ESPERT_DIR}/libs/${GET_LIBRARY_LIB_DIR_NAME}/${CMAKE_BUILD_TYPE}
        )

        set(${GET_LIBRARY_NAME}_DIR ${ESPERT_DIR}/libs/${GET_LIBRARY_LIB_DIR_NAME}/${CMAKE_BUILD_TYPE}/${GET_LIBRARY_CONF_PATH})
        find_package(${GET_LIBRARY_NAME} 
            NAMES ${GET_LIBRARY_CONF_NAMES}
            NO_DEFAULT_PATH
            REQUIRED
        )
    endif()

endmacro()
