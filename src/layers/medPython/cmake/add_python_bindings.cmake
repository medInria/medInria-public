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

    cmake_parse_arguments(args
        ""
        "NAME"
        "INCLUDE;IMPORT"
        ${ARGN}
        )

    if (DEFINED args_NAME)
        set(module_name ${args_NAME})
    else()
        set(module_name ${bindings_target})
    endif()

    get_target_property(package_name ${cpp_target} PYTHON_PACKAGE_NAME)
    if (NOT package_name)
        set(package_name ${cpp_target})
    endif()

    set(bindings_dir ${CMAKE_CURRENT_BINARY_DIR}/swig/${bindings_target})
    set(interface_file ${bindings_dir}/${bindings_target}.i)
    set(modules_dir ${CMAKE_CURRENT_BINARY_DIR}/python/modules/${package_name})
    set(libraries_dir ${CMAKE_CURRENT_BINARY_DIR}/python/libraries/${package_name})

    string(CONCAT interface_content
        "%{\n"
        "   #include \"medPython.h\"\n"
        "%}\n\n"
        "%module(package=\"${package_name}\", directors=\"1\") ${module_name}\n\n"
        "%feature(\"director:except\")\n"
        "{\n"
        "  if ($error != nullptr)\n"
        "  {\n"
        "    med::python::propagateCurrentError()\;\n"
        "  }\n"
        "}\n\n"
        "%exception\n"
        "{\n"
        "  try\n"
        "  {\n"
        "    $action\n"
        "  }\n"
        "  catch (med::python::Exception e)\n"
        "  {\n"
        "    med::python::raiseError(e.nativeClass(), e.what())\;\n"
        "    SWIG_fail\;\n"
        "  }\n"
        "}\n\n"
        )

    foreach(import ${args_IMPORT})
        if (TARGET ${import})
            get_target_property(import ${import} SWIG_INTERFACE_FILE)
        endif()
        string(APPEND interface_content "%import \"${import}\"\n\n")
    endforeach()

    foreach(file ${args_INCLUDE})
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
        SWIG_COMPILE_DEFINITIONS SWIG_TYPE_TABLE=${PYTHON_PROJECT_NAME}
        SWIG_INTERFACE_FILE ${interface_file}
        )

    target_link_libraries(${bindings_target} PUBLIC ${PYTHON_PROJECT_NAME}Base)

    set_property(TARGET ${cpp_target} APPEND PROPERTY PYTHON_BINDINGS ${bindings_target})

    get_target_property(library_prefix ${bindings_target} PREFIX)

    if (WIN32)
        get_target_property(library_suffix ${bindings_target} SUFFIX)
    else()
        set(library_suffix ".so")
    endif()

    set(library ${libraries_dir}/${library_prefix}${module_name}${library_suffix})
    set_source_files_properties(${library} PROPERTIES GENERATED TRUE)

endfunction()
