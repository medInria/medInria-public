################################################################################
#
# medInria

# Copyright (c) INRIA 2013. All rights reserved.

# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

set(TARGET_NAME medMeshingWorkspacePlugin)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} MODULE
  "${${TARGET_NAME}_CFILES}"
  )

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${${PROJECT_NAME}_VERSION})
string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
target_compile_definitions(${TARGET_NAME} PUBLIC ${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

## #############################################################################
## source
## #############################################################################
target_sources(${TARGET_NAME}
    PRIVATE ${${TARGET_NAME}_SOURCES}
    PUBLIC ${${TARGET_NAME}_TEMPLATES}
    PUBLIC ${${TARGET_NAME}_HEADERS}
)
## #############################################################################
## include directories
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

target_include_directories(${TARGET_NAME}
  PRIVATE ${${TARGET_NAME}_INCLUDE_DIRS}
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  medCore
  medUtilities
  )

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
