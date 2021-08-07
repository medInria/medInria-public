################################################################################
#
# medInria
#
# Copyright (c) INRIA 2021. All rights reserved.
#
# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

function(add_python_bindings cpp_target bindings_target)

    cmake_parse_arguments(BINDINGS
        ""
        "NAME;REMOVE_BINDINGS_PREFIX;HEADER_FILE_PREFIX"
        "SIMPLE_BINDINGS;DIRECTOR_BINDINGS;INCLUDE_INTERFACES;IMPORT_INTERFACES;INCLUDE_HEADERS"
        ${ARGN}
        )

    if (DEFINED option_NAME)
        set(module_name ${option_NAME})
    else()
        set(module_name ${bindings_target})
    endif()

    set(bindings_dir ${CMAKE_CURRENT_BINARY_DIR}/swig/${bindings_target})
    set(interface_file ${bindings_dir}/${bindings_target}.i)
    set(modules_dir ${CMAKE_CURRENT_BINARY_DIR}/python/modules/${cpp_target})
    set(libraries_dir ${CMAKE_CURRENT_BINARY_DIR}/python/libraries/${cpp_target})

    string(CONCAT interface_content
        "%include \"standard_defs.i\"\n\n"
        "%module(package=\"${cpp_target}\", directors=\"1\") ${module_name}\n\n"
        "%feature(\"director:except\")\n"
        "{\n"
        "  if ($error != nullptr)\n"
        "  {\n"
        "    throw Swig::DirectorMethodException()\;\n"
        "  }\n"
        "}\n\n"
        )

    foreach(file ${option_INCLUDE_HEADERS})
        string(APPEND interface_content "%{\n#include \"${file}\"\n%}\n\n")
    endforeach()

    foreach(file ${option_IMPORT_INTERFACES})
        string(APPEND interface_content "%import \"${file}\"\n\n")
    endforeach()

    foreach(binding ${option_DIRECTOR_BINDINGS})
        string(APPEND "%feature(\"director\") ${binding}\;\n\n")
    endforeach()

    foreach(binding ${option_SIMPLE_BINDINGS} ${option_DIRECTOR_BINDINGS})
        if (DEFINED option_REMOVE_BINDINGS_PREFIX)
            STRING(REGEX REPLACE "^${option_REMOVE_BINDINGS_PREFIX}" "" unprefixed_binding ${binding})
            string(APPEND interface_content "%rename(${unprefixed_binding}) ${binding}\;\n\n")
        endif()

        string(APPEND interface_content
            "%{\n"
            "#include \"${option_HEADER_FILE_PREFIX}${binding}.h\"\n"
            "%}\n\n"
            "%include \"${option_HEADER_FILE_PREFIX}${binding}.h\"\n\n"
            )
    endforeach()

    foreach(file ${option_INCLUDE_INTERFACES})
        string(APPEND interface_content "%include \"${file}\"\n\n")
    endforeach()

    file(WRITE ${interface_file} ${interface_content})

    set_source_files_properties(${interface_file}
        PROPERTIES CPLUSPLUS ON
        )

    swig_add_library(${bindings_target}
        TYPE MODULE
        LANGUAGE python
        OUTPUT_DIR ${modules_dir}
        OUTFILE_DIR ${bindings_dir}
        SOURCES ${interface_file}
        )

    set_target_properties(${bindings_target} PROPERTIES
        SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE
        LIBRARY_OUTPUT_DIRECTORY ${libraries_dir}
        OUTPUT_NAME ${module_name}
        )

    target_include_directories(${bindings_target}
        PUBLIC
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts
        )

    target_link_libraries(${bindings_target} PUBLIC medPython)

    set_property(TARGET ${cpp_target}
        APPEND PROPERTY PYTHON_BINDINGS ${bindings_target}
        )

    get_target_property(library_prefix ${bindings_target} PREFIX)

    if (WIN32)
        get_target_property(library_suffix ${bindings_target} SUFFIX)
    else()
        set(library_suffix ".so")
    endif()

    set(library ${libraries_dir}/${library_prefix}${module_name}${library_suffix})

    set_source_files_properties(${library} PROPERTIES GENERATED TRUE)
    add_custom_command(OUTPUT ${library} COMMAND DEPENDS ${bindings_target})

endfunction()
