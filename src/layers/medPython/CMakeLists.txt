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

if (UNIX)
    if (APPLE)
        set(minor_python_version 9)
    else()
        set(minor_python_version 8)
    endif()

    set(python_library_name "python3.${minor_python_version}")

elseif (WIN32)
    set(minor_python_version 9)
    set(python_library_name "python.exe")
endif()

set(python_working_dir "${CMAKE_CURRENT_BINARY_DIR}/python")

if (NOT EXISTS "${python_working_dir}")

    set(embedded_python_url "https://med.inria.fr/downloads/embedded-python")
    set(embedded_python_zip "${python_working_dir}/Python.zip")

    if (UNIX)
        if (APPLE)
            set(embedded_python_url "${embedded_python_url}/python-3.${minor_python_version}-mac.zip")
        else()
            set(embedded_python_url "${embedded_python_url}/python-3.${minor_python_version}-linux.zip")
        endif()
    elseif (WIN32)
        set(embedded_python_url "${embedded_python_url}/python-3.${minor_python_version}-win.zip")
    endif()

    message("Downloading the embeddable Python package from ${embedded_python_url}")
    file(DOWNLOAD ${embedded_python_url} "${embedded_python_zip}" STATUS download_status)
    list(POP_FRONT download_status download_code download_error)

    if (download_code)
        message(WARNING "Downloading failed: ${download_error} (${download_code})")
    else()
        message("Download succeeded")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar x "${embedded_python_zip}"
            WORKING_DIRECTORY "${python_working_dir}"
            )
        file(REMOVE "${embedded_python_zip}")
    endif()

endif()

file(GLOB python_files LIST_DIRECTORIES FALSE "${python_working_dir}/*")

if (UNIX)
    file(GLOB_RECURSE python_lib_files LIST_DIRECTORIES FALSE "${python_working_dir}/lib/*")
    list(APPEND python_files ${python_lib_files})
endif()

foreach (python_file ${python_files})
    add_external_resources(${TARGET_NAME} "${python_file}")
endforeach()

target_include_directories(${TARGET_NAME} PUBLIC "${python_working_dir}/include/python3.${minor_python_version}")

target_compile_definitions(${TARGET_NAME} PUBLIC
    PYTHON_VERSION_MAJOR=3
    PYTHON_VERSION_MINOR=${minor_python_version}
    PYTHON_LIBRARY="${python_library_name}"
    STANDARD_PYTHON_MODULES_ZIP="python3${minor_python_version}.zip"
    )

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
