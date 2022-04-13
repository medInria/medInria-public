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

function(add_python_module target_name)

    cmake_parse_arguments(PARSE_ARGV 1 "ARG"
        ""
        "PACKAGE;PARENT_PACKAGE;BINDINGS_MODULE_NAME;BINDINGS_LIBRARY_NAME"
        "SOURCES"
        )

    if (NOT ARG_SOURCES)
        message(FATAL_ERROR "No sources specified.")
    endif()

    _split_source_files(python_sources cpp_sources swig_sources ${ARG_SOURCES})
    set(resource_dir lib)

    if (ARG_PARENT_PACKAGE)
        get_target_property(parent_package_name ${ARG_PARENT_PACKAGE} ${PROJECT_NAME}_PYTHON_PACKAGE_NAME)
        string(REGEX REPLACE "\\." / parent_package_dir ${parent_package_name})
        string(APPEND resource_dir /${parent_package_dir})
    endif()

    if (ARG_PACKAGE)
        string(APPEND resource_dir /${ARG_PACKAGE})
    else()
        if (python_sources)
            list(LENGTH python_sources num_modules)
            if (swig_sources)
                math(EXPR num_modules "${num_modules} + 1")
            endif()
            if (num_modules GREATER 1)
                message(FATAL_ERROR "The PACKAGE option is required as there are more than one modules.")
            endif()
        endif()
    endif()

    if (swig_sources)
        if (ARG_BINDINGS_MODULE_NAME)
            set(bindings_module_name ${ARG_BINDINGS_MODULE_NAME})
        else()
            if (ARG_PACKAGE)
                set(bindings_module_name bindings)
            else()
                set(bindings_module_name ${target_name})
            endif()
        endif()
        if (ARG_BINDINGS_LIBRARY_NAME)
            set(bindings_library_name ${ARG_BINDINGS_LIBRARY_NAME})
        else()
            if (ARG_PACKAGE AND NOT ARG_BINDINGS_MODULE_NAME)
                set(bindings_library_name ${ARG_PACKAGE}_bindings)
            else()
                set(bindings_library_name ${bindings_module_name})
            endif()
        endif()
        _add_bindings_module(${target_name} ${bindings_module_name} ${bindings_library_name} "${resource_dir}" "${swig_sources}" "${cpp_sources}")
        if(ARG_PACKAGE)
            get_target_property(prefix ${target_name} PREFIX)
            set(forward_module "${CMAKE_CURRENT_BINARY_DIR}/swig/${prefix}${bindings_library_name}.py")
            file(WRITE ${forward_module} "from ${prefix}${bindings_library_name} import *")
            list(APPEND python_sources "${forward_module}")
        endif()
        list(APPEND python_sources "${CMAKE_CURRENT_BINARY_DIR}/swig/${bindings_module_name}.py")
    else()
        add_custom_target(${target_name} ALL)
    endif()

    set_external_resources(${target_name} APPEND
        DESTINATION ${resource_dir}
        FILES ${python_sources}
        )

    if (ARG_PACKAGE)
        set_target_properties(${target_name} PROPERTIES ${PROJECT_NAME}_PYTHON_PACKAGE_NAME ${ARG_PACKAGE})
    endif()

    target_sources(${target_name} PUBLIC ${ARG_SOURCES})

    if (ARG_PARENT_PACKAGE)
        set_property(TARGET ${ARG_PARENT_PACKAGE} APPEND PROPERTY ${PROJECT_NAME}_PYTHON_PACKAGE_SUB_TARGETS ${target_name})
    endif()

endfunction()

macro(_split_source_files python_files_var cpp_files_var swig_files_var)

    set(${python_files_var})
    set(${cpp_files_var})
    set(${swig_files_var})

    foreach (file ${ARGN})
        get_filename_component(extension "${file}" EXT)
        get_filename_component(filename "${file}" NAME_WE)

        if (extension STREQUAL ".py")
            list(APPEND ${python_files_var} "${file}")
        elseif (extension STREQUAL ".py.in")
            configure_file("${file}" "${CMAKE_CURRENT_BINARY_DIR}/${filename}.py" @ONLY)
            list(APPEND ${python_files_var} "${CMAKE_CURRENT_BINARY_DIR}/${filename}.py")
        elseif (extension STREQUAL ".i")
            list(APPEND ${swig_files_var} "${file}")
        elseif (extension STREQUAL ".i.in")
            configure_file("${file}" "${CMAKE_CURRENT_BINARY_DIR}/swig/${filename}.i" @ONLY)
            list(APPEND ${swig_files_var} "${CMAKE_CURRENT_BINARY_DIR}/swig/${filename}.i")
        else()
            list(APPEND ${cpp_files_var} "${file}")
        endif()
    endforeach()
endmacro()

function(_add_bindings_module target_name module_name library_name resource_dir swig_sources cpp_sources)

    set(working_dir "${CMAKE_CURRENT_BINARY_DIR}/swig/${target}")

    set(main_swig_source)
    foreach (file ${swig_sources})
        get_filename_component(filename "${file}" NAME_WE)
        if (filename STREQUAL ${module_name})
            set_source_files_properties("${file}" PROPERTIES
                CPLUSPLUS ON
                USE_SWIG_DEPENDENCIES ON
                SWIG_MODULE_NAME ${module_name}
                )
            set(main_swig_source "${file}")
        endif()
    endforeach()

    swig_add_library(${target_name}
        TYPE SHARED
        LANGUAGE python
        OUTPUT_DIR "${working_dir}"
        OUTFILE_DIR "${working_dir}"
        SOURCES ${main_swig_source} ${cpp_sources}
        )

    set_target_properties(${target_name} PROPERTIES
        SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE
        SWIG_COMPILE_DEFINITIONS SWIG_TYPE_TABLE=${PYTHON_PROJECT_NAME}
        SWIG_COMPILE_OPTIONS -py3
        OUTPUT_NAME ${library_name}
        )

    set_lib_install_rules(${target_name} RESOURCE)

    target_include_directories(${target_name}
        PUBLIC
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts
        "${working_dir}"
        )

endfunction()

function(_forward_bindings_module)

endfunction()
