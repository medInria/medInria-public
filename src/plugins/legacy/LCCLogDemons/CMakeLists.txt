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

set(TARGET_NAME LCCLogDemonsPlugin)


## #############################################################################
## find required
## #############################################################################

if (UNIX AND NOT APPLE)
# Don't export unnecessary symbols, otherwise possible crash on exit
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--exclude-libs,ALL")
endif()

#find_package(Qt5 COMPONENTS REQUIRED Core)

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(ITK REQUIRED COMPONENTS ITKCommon ITKTransform ITKRegistrationCommon ITKImageGrid ITKImageFilterBase  ITKIOImageBase ITKTransformFactory ITKIOTransformBase ITKIOMeta ITKIOTransformInsightLegacy ITKPDEDeformableRegistration ITKReview)
include(${ITK_USE_FILE})

find_package(RPI REQUIRED)
include(${RPI_USE_FILE})

find_package(LogDemons REQUIRED)
include(${LogDemons_USE_FILE})

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
  ${ITKIO_LIBRARIES}
  ITKPDEDeformableRegistration
  Qt5::Core
  dtkLog
  dtkCoreSupport
  medCoreLegacy
  medRegistration  
  RPILCClogDemons
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## #################################################################
## Install rules
## #################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})