cmake_policy(SET CMP0115 OLD) 

set(source_files
    main.dox
    build.dox
    libraries.dox
    systems.dox
)

file(GLOB_RECURSE header_paths ${ESPERT_DIR}/src/*.hh)

foreach(path IN LISTS header_paths)
    string(APPEND ESP_DOXYGEN_INPUT " \\\n\"${path}\"")
endforeach()
foreach(file IN LISTS source_files)
    string(APPEND ESP_DOXYGEN_INPUT " \\\n\"${CMAKE_CURRENT_SOURCE_DIR}/${file}\"")
endforeach()

configure_file(Doxyfile.in Doxyfile @ONLY)

add_custom_command(OUTPUT "html/index.html"
                   COMMAND "${DOXYGEN_EXECUTABLE}"
                   WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                   MAIN_DEPENDENCY Doxyfile
                   DEPENDS ${header_paths} ${source_files}
                   COMMENT "Generating HTML documentation"
                   VERBATIM)

add_custom_target(docs ALL SOURCES "html/index.html")
set_target_properties(docs PROPERTIES FOLDER "espert")

