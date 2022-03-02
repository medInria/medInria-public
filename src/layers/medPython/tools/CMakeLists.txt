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

set(TARGET_NAME ${PYTHON_PROJECT_NAME}Tools)
set(PYTHON_PACKAGE_NAME ${TARGET_NAME})

## #############################################################################
## List sources
## #############################################################################

list_source_files(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

list_header_directories_to_include(${TARGET_NAME}
    ${${TARGET_NAME}_HEADERS}
    )

set(qt_bindings_sources
    qt/header.i
    qt/string.i
    qt/variant.i
    qt/list.i
    qt/hash.i
    qt/base.i
    qt/object.i
    qt/widgets.i
    qt/application.i
    )

set(medInria_bindings_sources
    medInria/header.i
    medInria/data.i
    medInria/gui.i
    )

## #############################################################################
## Add targets
## #############################################################################

add_library(${TARGET_NAME} SHARED
    ${${TARGET_NAME}_CFILES}
    )

target_compile_definitions(${TARGET_NAME} PRIVATE
    TARGET_NAME="${TARGET_NAME}"
    PYTHON_PACKAGE_NAME="${PYTHON_PACKAGE_NAME}"
    )

## #############################################################################
## Add Python modules
## #############################################################################

set_property(TARGET ${TARGET_NAME} PROPERTY
    PYTHON_PACKAGE_NAME ${PYTHON_PACKAGE_NAME})

add_python_bindings(${TARGET_NAME} qt_bindings
    INCLUDE
    ${qt_bindings_sources}
    )

add_python_bindings(${TARGET_NAME} medInria_bindings
    IMPORT
    qt_bindings
    INCLUDE
    ${medInria_bindings_sources}
    )

add_python_modules(${TARGET_NAME}
    SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python
    __init__.py
    console.py
    )

generate_python_resources(${TARGET_NAME})

## #############################################################################
## Include directories
## #############################################################################

target_include_directories(${TARGET_NAME}
    PUBLIC
    ${${TARGET_NAME}_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings
    )

target_include_directories(qt_bindings
    PUBLIC
    ${${TARGET_NAME}_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings
    )

target_include_directories(medInria_bindings
    PUBLIC
    ${${TARGET_NAME}_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings
    )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
    PUBLIC
    ${PYTHON_PROJECT_NAME}Base
    )

target_link_libraries(qt_bindings
    PUBLIC
    Qt5::Widgets
    )

target_link_libraries(medInria_bindings
    PUBLIC
    Qt5::Widgets
    medCoreLegacy
    )

## #############################################################################
## Install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
    ${${TARGET_NAME}_HEADERS}
    )
