macro(get_library)
    set(oneValueArgs TARGET PATH)
    set(multiValueArgs NAMES ARGS)
    cmake_parse_arguments(GET_LIBRARY "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})


    set(${GET_LIBRARY_TARGET}_DIR ${ESPERT_DIR}/libs/${GET_LIBRARY_TARGET}/${CMAKE_BUILD_TYPE}/${GET_LIBRARY_PATH})
    find_package(${GET_LIBRARY_TARGET} 
        NAMES ${GET_LIBRARY_NAMES}
        NO_DEFAULT_PATH
    )

    if(NOT ${GET_LIBRARY_TARGET}_FOUND)
        # execute_process(COMMAND python3 ${ESPERT_DIR}/scripts/install-library.py ${GET_LIBRARY_TARGET} ${GET_LIBRARY_ARGS}
        #     RESULT_VARIABLE PROC_RESULT
        # )

        # if(NOT ${PROC_RESULT} EQUAL 0)
        #     message(FATAL_ERROR "Could not build ${GET_LIBRARY_TARGET}")
        # endif()

        include(${ESPERT_DIR}/cmake/install-library.cmake)
        install_library(NAME ${GET_LIBRARY_TARGET}
            ARGS ${GET_LIBRARY_ARGS}
        )

    endif()

    set(${GET_LIBRARY_TARGET}_DIR ${ESPERT_DIR}/libs/${GET_LIBRARY_TARGET}/${CMAKE_BUILD_TYPE}/${GET_LIBRARY_PATH})
    find_package(${GET_LIBRARY_TARGET} 
        NAMES ${GET_LIBRARY_NAMES}
        NO_DEFAULT_PATH
        REQUIRED
    )

endmacro(get_library)
