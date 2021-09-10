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

set(TARGET_NAME ${PYTHON_PROJECT_NAME}Base)

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
## Generate SWIG Runtime header
## #############################################################################

set(SWIG_RUNTIME_HEADER_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/swig)
set(SWIG_RUNTIME_HEADER ${SWIG_RUNTIME_HEADER_DIRECTORY}/swig_runtime.h)

add_custom_command(
    OUTPUT ${SWIG_RUNTIME_HEADER}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SWIG_RUNTIME_HEADER_DIRECTORY}
    COMMAND ${SWIG_EXECUTABLE}
    ARGS
    "-python"
    "-c++"
    "-external-runtime"
    ${SWIG_RUNTIME_HEADER}
    )

## #############################################################################
## Add targets
## #############################################################################

add_library(${TARGET_NAME} SHARED
    ${${TARGET_NAME}_CFILES}
    ${SWIG_RUNTIME_HEADER}
    )

target_compile_definitions(${TARGET_NAME} PRIVATE
    TARGET_NAME="${TARGET_NAME}"
    PYTHON_PLUGIN_PREFIX="${PYTHON_PLUGIN_PREFIX}"
    )

## #############################################################################
## Include directories
## #############################################################################

target_include_directories(${TARGET_NAME}
    PUBLIC
    ${${TARGET_NAME}_INCLUDE_DIRS}
    ${SWIG_RUNTIME_HEADER_DIRECTORY}
    )

## #############################################################################
## Embed Python
## #############################################################################

embed_python(${TARGET_NAME})

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
