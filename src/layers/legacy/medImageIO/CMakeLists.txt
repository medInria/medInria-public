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

set(TARGET_NAME medImageIO)


## #############################################################################
## find requireds
## #############################################################################

find_package(ITK REQUIRED COMPONENTS ITKVtkGlue ITKIOImageBase ITKIOMeta ITKZLIB)
include(${ITK_USE_FILE})

#WARNING USE VTK through ITKVtkGlue and medLog

find_package(DCMTK REQUIRED NO_MODULE)


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
  )

## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PUBLIC ${${TARGET_NAME}_INCLUDE_DIRS}
  PRIVATE ${DCMTK_INCLUDE_DIRS}
  )


## #############################################################################
## Link
## #############################################################################


target_link_libraries(${TARGET_NAME}
  Qt5::Core
  Qt5::Widgets
  dtkCoreSupport
  dtkLog
  medCore
  medCoreLegacy
  ITKCommon
  ITKIOImageBase
  ITKIOBMP
  ITKIOBruker
  ITKIOLSM
  ITKIOJPEG
  ITKIOJPEG2000
  ITKIOPNG
  ITKIOVTK
  ITKIOGDCM
  ITKNrrdIO
  ITKIONRRD
  ITKIOMeta
  ITKIOMINC
  ITKIONIFTI
  ITKIOGIPL
  ITKIOGE
  ITKIOHDF5
  ITKIOBioRad
  ITKIOStimulate
  ITKIOMRC
  ITKIOTIFF
  ITKVTK
  vtkIOImage
  ${ITKIOPhilipsREC_LIBRARIES}
  ${DCMTK_LIBRARIES}
  )

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME} HEADERS
  ${${TARGET_NAME}_HEADERS}
  )
