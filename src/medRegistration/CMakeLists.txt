################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

project(medRegistration)


## #############################################################################
## find requireds
## #############################################################################

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})


find_package(RPI REQUIRED)
include(${RPI_USE_FILE})


## #############################################################################
## List Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )


## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${medCore_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
  )
    
## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_CFILES}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}
    ${QT_LIBRARIES}
  dtkCoreSupport
  medCore
  dtkLog
  RegistrationAddOn
  ITKCommon
  ITKIOImageBase
  ITKIOBMP
  ITKIOLSM
  ITKIOJPEG
  ITKIOGE
  ITKIOPNG
  ITKIOVTK
  ITKIOGDCM
  ITKNrrdIO
  ITKIONRRD
  ITKIOMeta
  ITKIONIFTI
  ITKIOGIPL
  ITKIOHDF5
  ITKIOBioRad
  ITKIOStimulate
  ITKStatistics
  ITKIOTransformBase
  ITKIOTransformHDF5
  ITKIOTransformMatlab
  ITKIOTransformInsightLegacy
  ITKIOMRC
  )


## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )
