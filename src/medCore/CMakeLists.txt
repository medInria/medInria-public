################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

set(TARGET_NAME medCore)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  process
  process/arithmetic_operation
  process/segmentation
  gui
  gui/workspace
  gui/area
  )


## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${medCoreLegacy_INCLUDE_DIRS}
  )


## #############################################################################
## add library
## #############################################################################

qt5_add_resources(${TARGET_NAME}_QRC
  ${${TARGET_NAME}_QRC}
  )

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
    ${QT_LIBRARIES}
    dtkCoreSupport
    dtkGuiSupport
    dtkLog
    dtkComposer
    )

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )
