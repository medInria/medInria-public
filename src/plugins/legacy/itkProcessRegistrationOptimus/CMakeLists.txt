################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2020. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

set(TARGET_NAME "itkProcessRegistrationOptimusPlugin")

## #################################################################
## Setup version numbering
## #################################################################

set(${TARGET_NAME}_VERSION ${${PROJECT_NAME}_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")


## #############################################################################
## find required
## #############################################################################

find_package(ITK REQUIRED  COMPONENTS ITKCommon ITKTransform  ITKRegistrationCommon ITKIOImageBase ITKIOMeta ITKTransformFactory ITKIOTransformBase ITKIOTransformInsightLegacy)
include(${ITK_USE_FILE})

find_package(RPI REQUIRED)
include(${RPI_USE_FILE})

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

## #############################################################################
## include directories
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  )

## #################################################################
## Build rules
## #################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )

## #############################################################################
## Link
## #############################################################################

SET(ITKIO_LIBRARIES
  ITKIOImageBase
  ITKIOBMP
  ITKIOBioRad
  ITKIOHDF5
  ITKIOGDCM
  ITKIOGE
  ITKIOGIPL
  ITKIOJPEG
  ITKIOLSM
  ITKIOMeta
  ITKIONIFTI
  ITKIONRRD
  ITKIOPNG
  ITKIOStimulate
  ${ITKIOPhilipsREC_LIBRARIES}
  ITKIOTIFF
  ITKIOVTK
  ITKIOTransformBase
  ITKIOTransformInsightLegacy
  ITKIOTransformHDF5
  ITKIOTransformMatlab
  ITKIOMRC
)

target_link_libraries(${TARGET_NAME}
  Qt5::Core
  dtkLog
  dtkCoreSupport
  medCoreLegacy
  ${ITKIO_LIBRARIES}
  ITKOptimizers
  ITKSmoothing
  medRegistration
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## #################################################################
## Install rules
## #################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})