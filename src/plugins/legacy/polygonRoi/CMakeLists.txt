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

set(TARGET_NAME polygonRoiPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(dtk REQUIRED
    dtkCore
    dtkCoreSupport
    )
include_directories(${dtk_INCLUDE_DIRS})

find_package(ITK REQUIRED COMPONENTS ITKCommon ITKImageLabel ITKVtkGlue ITKIOImageBase ITKIOMeta)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED COMPONENTS vtkGUISupportQt vtkCommonCore vtkIOLegacy)
include(${VTK_USE_FILE})

find_package(Qt5 REQUIRED COMPONENTS Gui)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  data
  readers
  writers
  viewinteractors
  toolboxes
  json
  viewevent
  )

## #############################################################################
## include directories
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  ${DATA_HEADERS}
  )

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ${QT_LIBRARIES}
  dtkCore
  dtkCoreSupport
  medCore
  medRegistration
  medVtkInria
  medUtilities
  VTK::CommonComputationalGeometry
  )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
