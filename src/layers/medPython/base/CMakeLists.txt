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

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    )

## #############################################################################
## Find dependencies
## #############################################################################

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(Qt5 REQUIRED COMPONENTS Core)

include(python_info)

## #############################################################################
## List sources
## #############################################################################

list_source_files(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/conversion
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/error
    ${CMAKE_CURRENT_SOURCE_DIR}/objects
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
## Download embeddable Python
## #############################################################################

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/python")
set(python_zip "${CMAKE_CURRENT_BINARY_DIR}/python/python.zip")

add_custom_command(OUTPUT "${python_zip}"
    COMMAND ${CMAKE_COMMAND} ARGS
    -D PYTHON_ZIP=${python_zip}
    -D PYTHON_VERSION_MINOR=${PYTHON_VERSION_MINOR}
    -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/download_python.cmake"
    VERBATIM
    )

## #############################################################################
## List Python files
## #############################################################################

foreach (file ${PYTHON_HEADERS})
    list(APPEND python_headers "${CMAKE_CURRENT_BINARY_DIR}/python/${PYTHON_HEADERS_DIR}/${file}")
endforeach()

foreach (file ${PYTHON_LIBRARIES})
    list(APPEND python_libraries "${CMAKE_CURRENT_BINARY_DIR}/python/${PYTHON_LIBRARIES_DIR}/${file}")
endforeach()

foreach (file ${PYTHON_MODULES})
    list(APPEND python_modules "${CMAKE_CURRENT_BINARY_DIR}/python/${PYTHON_MODULES_DIR}/${file}")
endforeach()

set(python_license_file "${CMAKE_CURRENT_BINARY_DIR}/python/${PYTHON_LICENSE_FILE}")

## #############################################################################
## Extract Python files
## #############################################################################

add_custom_command(OUTPUT ${python_headers} ${python_libraries}  ${python_modules}  ${python_license_file}
    COMMAND ${CMAKE_COMMAND} ARGS
    -D PYTHON_ZIP=${python_zip}
    -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/extract_python.cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/python"
    DEPENDS ${python_zip}
    VERBATIM
    )

## #############################################################################
## Python-related compilation settings
## #############################################################################

# To prevent warning of generated files not being moc'd
cmake_policy(SET CMP0071 NEW)

target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/python/${PYTHON_HEADERS_DIR}")
target_sources(${TARGET_NAME} PRIVATE ${python_headers})

target_compile_definitions(${TARGET_NAME} PUBLIC
    PYTHON_VERSION_MINOR=${PYTHON_VERSION_MINOR}
    PYTHON_MAIN_LIBRARY="${PYTHON_MAIN_LIBRARY}"
    )

## #############################################################################
## Add embedded Python files as resources
## #############################################################################

add_external_resources(${TARGET_NAME} ${python_libraries}  ${python_modules}  ${python_license_file})

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
