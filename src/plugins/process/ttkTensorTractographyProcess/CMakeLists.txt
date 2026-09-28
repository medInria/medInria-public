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

set(TARGET_NAME ttkTensorTractographyProcessPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(VTK REQUIRED CommonCore CommonDataModel)

find_package(ITK REQUIRED ITKCommon ITKTransform ITKImageFunction)
include(${ITK_USE_FILE})

find_package(TTK REQUIRED COMPONENTS ITKTensor ttkAlgorithms)

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
## include directories.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PRIVATE ${${TARGET_NAME}_INCLUDE_DIRS}
  )
# PRIVATE because plugins are not supposed to be used somewhere else

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ITKCommon
  Qt5::Core
  dtkCore
  dtkCoreSupport
  dtkLog
  TTK::ITKTensor
  TTK::ttkAlgorithms
  ITKIOTransformBase
  medCore
  medCoreLegacy
  medVtkInria
  medWidgets
  ${VTK_LIBRARIES}
  )

vtk_module_autoinit(TARGETS ${TARGET_NAME} MODULES ${VTK_LIBRARIES})

## #################################################################
## Install rules
## #################################################################

set_plugin_install_rules(${TARGET_NAME})
