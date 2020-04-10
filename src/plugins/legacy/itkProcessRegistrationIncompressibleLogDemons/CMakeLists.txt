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

set(TARGET_NAME itkProcessRegistrationILogDemonsPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

find_package(ITK REQUIRED COMPONENTS ITKCommon ITKTransform ITKTransformFactory ITKRegistrationCommon ITKIOMeta)
include(${ITK_USE_FILE})

find_package(TTK REQUIRED)
include(${TTK_USE_FILE})

find_package(RPI REQUIRED)
include(${RPI_USE_FILE})

SET(ITKIO_LIBRARIES
  ITKIOImageBase
  ITKIOBMP
  ITKIOBioRad
  ITKIOBruker
  ITKIOHDF5
  ITKIOGDCM
  ITKIOGE
  ITKIOGIPL
  ITKIOJPEG
  ITKIOJPEG2000
  ITKIOLSM
  ITKIOMeta
  ITKIOMINC
  ITKIONIFTI
  ITKIONRRD
  ITKIOPNG
  ${ITKIOPhilipsREC_LIBRARIES}
  ITKIOTIFF
  ITKIOStimulate
  ITKIOVTK
  ITKIOTransformBase
  ITKIOTransformInsightLegacy
  ITKIOTransformHDF5
  ITKIOTransformMatlab
  ITKIOMRC
)

## #################################################################
## Input
## #################################################################

list_source_files(${TARGET_NAME}
  .
  )

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

## #################################################################
## Build rules
## #################################################################

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
  Qt5::Core
  dtkCoreSupport
  dtkLog
  medCoreLegacy
  ${ITKIO_LIBRARIES}
  ITKStatistics
  ITKTensor
  InrimageNG
  medRegistration
  LinearAlgebra
  Basics
  #mipsZ
)

if (PLUGINS_LEGACY_PATH)
  set_property(TARGET ${TARGET_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${PLUGINS_LEGACY_PATH})
  set_property(TARGET ${TARGET_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${PLUGINS_LEGACY_PATH})
endif()

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${TARGET_NAME}
  RUNTIME DESTINATION plugins_legacy
  LIBRARY DESTINATION plugins_legacy
  ARCHIVE DESTINATION plugins_legacy)
