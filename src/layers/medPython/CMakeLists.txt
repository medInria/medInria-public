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

set(TARGET_NAME medPython)

## #############################################################################
## Find dependencies
## #############################################################################

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(Qt5 REQUIRED COMPONENTS Core)

find_package(Python3 3.7 REQUIRED COMPONENTS Interpreter Development)
include_directories(${Python3_INCLUDE_DIRS})

## #############################################################################
## List sources
## #############################################################################

list_source_files(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    )

list_header_directories_to_include(${TARGET_NAME}
    ${${TARGET_NAME}_HEADERS}
    )

## #############################################################################
## Add targets
## #############################################################################

add_library(${TARGET_NAME} SHARED
    ${${TARGET_NAME}_CFILES}
    )

target_compile_definitions(${TARGET_NAME} PUBLIC TARGET_NAME="${TARGET_NAME}")

## #############################################################################
## Include directories
## #############################################################################

target_include_directories(${TARGET_NAME}
    PUBLIC
    ${${TARGET_NAME}_INCLUDE_DIRS}
    )

## #############################################################################
## Embed Python
## #############################################################################

target_compile_definitions(${TARGET_NAME} PUBLIC
    PYTHON_VERSION_MAJOR=${Python3_VERSION_MAJOR}
    PYTHON_VERSION_MINOR=${Python3_VERSION_MINOR}
    )

set(python_working_dir "${CMAKE_CURRENT_BINARY_DIR}/python")
make_directory("${python_working_dir}")

# Note: On macOS we need to copy the executable and main libraries in order to
# modify them with install_name_tool before adding them as resources.

## EXECUTABLE ##

if (APPLE)
    get_filename_component(executable_name "${Python3_EXECUTABLE}" NAME)
    set(python_executable "${python_working_dir}/${executable_name}")

    add_custom_command(
        OUTPUT "${python_executable}"
        COMMAND ${CMAKE_COMMAND} -E copy "${Python3_EXECUTABLE}" "${python_executable}"
        )
else()
    set(python_executable "${Python3_EXECUTABLE}")
endif()

add_external_resources(${TARGET_NAME} "${python_executable}")

target_compile_definitions(${TARGET_NAME} PUBLIC
    PYTHON_EXECUTABLE_NAME="${executable_name}"
    )

## MAIN LIBRARIES ##

foreach(python_library ${Python3_LIBRARIES})
    get_filename_component(python_library "${python_library}" REALPATH)

    if (APPLE)
        get_filename_component(library_name "${python_library}" NAME)
        set(copied_library "${python_working_dir}/${library_name}")
        set(library_id "@loader_path/${library_name}")

        add_custom_command(
            OUTPUT "${copied_library}"
            COMMAND ${CMAKE_COMMAND} -E copy "${python_library}" "${copied_library}"
            COMMAND ${CMAKE_INSTALL_NAME_TOOL} -id "${library_id}" "${copied_library}"
            COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change "${python_library}" "${library_id}" "${python_executable}")

        add_external_resources(${TARGET_NAME} "${copied_library}")
    else()
        add_external_resources(${TARGET_NAME} "${python_library}")
    endif()
endforeach()

## STANDARD MODULES ##

# The standard Python modules are copied and archived into a ZIP file (Python
# accepts ZIP archives in the module search paths). This allows us to preserve
# the directory structure since the resource system only permits flat layouts.

set(standard_module_files "")
file(GLOB files "${Python3_STDLIB}/*")

foreach(file ${files})
    if (NOT IS_DIRECTORY ${file})
        list(APPEND standard_module_files ${file})
    elseif ((NOT file STREQUAL "${Python3_STDLIB}/test")
           AND
           (EXISTS "${file}/__init__.py"))

        file(GLOB_RECURSE sub_files "${file}/*")

        foreach (sub_file ${sub_files})
            if (NOT sub_file MATCHES ".*__pycache__.*")
                list(APPEND standard_module_files ${sub_file})
            endif()
        endforeach()
    endif()
endforeach()

set(copied_modules_dir "${python_working_dir}/standard_modules")
make_directory("${copied_modules_dir}")

foreach(file ${standard_module_files})
    file(RELATIVE_PATH relative_file "${Python3_STDLIB}" "${file}")
    get_filename_component(relative_path "${relative_file}" DIRECTORY)
    file(COPY ${file} DESTINATION "${copied_modules_dir}/${relative_path}")
endforeach()

set(standard_modules_zip_name "standard_modules.zip")
set(standard_modules_zip "${python_working_dir}/${standard_modules_zip_name}")

add_custom_command(
    OUTPUT "${standard_modules_zip}"
    COMMAND ${CMAKE_COMMAND} -E tar "cvf" "${standard_modules_zip}" --format=zip  --
    "${copied_modules_dir}/*"
    WORKING_DIRECTORY "${copied_modules_dir}"
    )

add_external_resources(${TARGET_NAME} "${standard_modules_zip}")

target_compile_definitions(${TARGET_NAME} PUBLIC
    STANDARD_PYTHON_MODULES_ZIP="${standard_modules_zip_name}"
    )

## STANDARD MODULE LIBRARIES ##

file(GLOB standard_libraries "${Python3_STDLIB}/lib-dynload/*")

foreach(standard_library ${standard_libraries})
    add_external_resources(${TARGET_NAME} "${standard_library}")
endforeach()

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
    PUBLIC
    Qt5::Core
    medCoreLegacy
    )

## #############################################################################
## Install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
    ${${TARGET_NAME}_HEADERS}
    )
