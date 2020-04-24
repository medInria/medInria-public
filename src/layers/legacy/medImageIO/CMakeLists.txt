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
set(ITK_IMAGES_DEPENDENCIES
    ITKIOImageBase
    ITKIOMeta
    #common formats for common images
    ITKIOBMP ITKIOJPEG ITKIOJPEG2000 ITKIOPNG ITKIOTIFF
    #Medical images formats
    ITKIOGIPL ITKIONRRD ITKIONIFTI ITKIOGIPL ITKIOPhilipsREC ITKIOGDCM ITKIOBioRad ITKIOBruker
    ITKNrrdIO ITKIOMINC ITKIOHDF5 ITKIOMRC ITKIOLSM
)

# ITKCommon ITKIOImageBase
find_package(ITK REQUIRED COMPONENTS ${ITK_IMAGES_DEPENDENCIES} ITKIOVTK ITKIOGIPL ITKIOGE
    ITKIOStimulate ITKVTK ITKVtkGlue )

# find vtk
find_package(VTK REQUIRED COMPONENTS vtkIOImage)

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
   "${${TARGET_NAME}_QRC}"
  )

## #############################################################################
## source
## #############################################################################
target_sources(${TARGET_NAME}
    PRIVATE ${${TARGET_NAME}_SOURCES}
    PUBLIC ${${TARGET_NAME}_TEMPLATES}
    PUBLIC ${${TARGET_NAME}_HEADERS}
)

## #############################################################################
## include directories.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PUBLIC ${${TARGET_NAME}_INCLUDE_DIRS}
  PRIVATE ${DCMTK_INCLUDE_DIRS}
  PUBLIC ${ITK_INCLUDE_DIRS}
  PRIVATE ${VTK_INCLUDE_DIRS}
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  Qt5::Core
  Qt5::Widgets
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES}
  dtkCoreSupport
  dtkLog
  medCore
  medCoreLegacy
  ${DCMTK_LIBRARIES}
  )

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )
