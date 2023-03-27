################################################################################
#
# medInria
#
# Copyright (c) INRIA. All rights reserved.
#
# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

set(TARGET_NAME itkINRDataImageWriterPlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${TARGET_NAME}_VERSION_MAJOR 3 CACHE STRING "${TARGET_NAME} major version number." FORCE)
set(${TARGET_NAME}_VERSION_MINOR 1 CACHE STRING "${TARGET_NAME} minor version number." FORCE)
set(${TARGET_NAME}_VERSION_BUILD 1 CACHE STRING "${TARGET_NAME} build version number." FORCE)
set(${TARGET_NAME}_VERSION "${${TARGET_NAME}_VERSION_MAJOR}.${${TARGET_NAME}_VERSION_MINOR}.${${TARGET_NAME}_VERSION_BUILD}")

mark_as_advanced(${TARGET_NAME}_VERSION_MAJOR)
mark_as_advanced(${TARGET_NAME}_VERSION_MINOR)
mark_as_advanced(${TARGET_NAME}_VERSION_BUILD)

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

## #################################################################
## Resolve dependencies
## #################################################################

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

SET(ITKIO_LIBRARIES
  ITKIOImageBase
  ITKIOBMP
  ITKIOBioRad
  ITKIOBruker
  ITKIOHDF5
  ITKIOGDCM
  ITKIOGIPL
  ITKIOJPEG
  ITKIOLSM
  ITKIOMeta
  ITKIOMINC
  ITKIONIFTI
  ITKIONRRD
  ITKIOPNG
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
  ITKCommon
  ${ITKIO_LIBRARIES}
  medImageIO
  dtkCoreSupport
  dtkLog
  medCoreLegacy)

## #################################################################
## Install rules
## #################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
