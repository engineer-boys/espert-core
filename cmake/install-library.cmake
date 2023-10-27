macro(install_library)
    set(oneValueArgs NAME)
    set(multiValueArgs)
    cmake_parse_arguments(INSTALL_LIBRARY "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(BUILD_TYPE "--debug")
    else()
        set(BUILD_TYPE "--release")
    endif()

    if (VKB_WSI_SELECTION)
        string(TOLOWER ${VKB_WSI_SELECTION} WSI)
        execute_process(COMMAND python3 ${ESPERT_DIR}/scripts/install-library.py --wsi ${WSI} ${BUILD_TYPE} ${INSTALL_LIBRARY_NAME}
        RESULT_VARIABLE PROC_RESULT
        )
    else()
        execute_process(COMMAND python3 ${ESPERT_DIR}/scripts/install-library.py ${BUILD_TYPE} ${INSTALL_LIBRARY_NAME}
            RESULT_VARIABLE PROC_RESULT
        )
    endif()

    if(NOT ${PROC_RESULT} EQUAL 0)
        message(FATAL_ERROR "Could not build ${INSTALL_LIBRARY_NAME}")
    endif()
endmacro()
