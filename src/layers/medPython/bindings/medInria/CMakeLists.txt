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

set(TARGET_NAME medInriaBindings)

## #############################################################################
## List sources
## #############################################################################

file(GLOB sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.i
    ${CMAKE_CURRENT_SOURCE_DIR}/*.i.in
    )

## #############################################################################
## Add targets
## #############################################################################

add_python_module(${TARGET_NAME}
    BINDINGS_MODULE_NAME medInria
    BINDINGS_LIBRARY_NAME ${TARGET_NAME}
    SOURCES ${sources}
    )

## #############################################################################
## Include directories
## #############################################################################

target_include_directories(${TARGET_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
    PUBLIC
    ${PYTHON_PROJECT_NAME}Base
    qtBindings
    medCoreLegacy
    )
