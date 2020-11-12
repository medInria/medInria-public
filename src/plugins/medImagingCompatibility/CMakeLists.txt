################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

set(TARGET_NAME medImagingCompatibilityPlugin)


## #############################################################################
## find requireds
## #############################################################################

find_package(dtk REQUIRED)

find_package(dtkImaging REQUIRED)
include_directories(${dtkImaging_INCLUDE_DIRS})

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

## #############################################################################
## add library
## #############################################################################


add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )


## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PUBLIC ${${TARGET_NAME}_INCLUDE_DIRS}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  Qt5::Core
  Qt5::Gui
  medCore
  dtkLog
  dtkComposer
  dtkCore
  dtkImaging
  # Legacy
  medCoreLegacy
  )


## #############################################################################
## install
## #############################################################################

set_plugin_install_rules(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )
