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

set(TARGET_NAME itkDWIBrainMaskCalculatorProcessPlugin)

## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(TTK REQUIRED COMPONENTS Registration ttkAlgorithms)

find_package(ITK REQUIRED COMPONENTS ITKCommon ITKImageStatistics ITKImageIntensity ITKSmoothing ITKThresholding ITKMathematicalMorphology ITKBinaryMathematicalMorphology ITKLabelVoting)
include(${ITK_USE_FILE})

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
  ITKStatistics
  ITKSmoothing
  TTK::Registration
  TTK::ttkAlgorithms
  Qt5::Core
  dtkCore
  dtkCoreSupport
  dtkLog
  medCoreLegacy
  medCore
  medWidgets
  )

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules(${TARGET_NAME})
