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

project(medImageIO)


## #############################################################################
## find requireds
## #############################################################################

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(DCMTK REQUIRED)


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
  ${DCMTK_INCLUDE_DIRS}
  ${medCore_INCLUDE_DIRS}
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
  dtkCore 
  dtkLog
  medCore
  ITKCommon
  ITKIOImageBase
  ITKIOBMP
  ITKIOLSM
  ITKIOJPEG
  ITKIOPNG
  ITKIOVTK
  ITKIOGDCM
  ITKNrrdIO
  ITKIONRRD
  ITKIOMeta
  ITKIONIFTI
  ITKIOGIPL
  ITKIOGE
  ITKIOHDF5
  ITKIOBioRad
  ITKIOStimulate
  ITKIOMRC
  ${ITKIOPhilipsREC_LIBRARIES}
  ITKIOTIFF
  ${DCMTK_LIBRARIES}
  )

if(APPLE)
  FixDCMTKMacLink(${PROJECT_NAME} ${DCMTK_LIBRARIES})
endif(APPLE)


## #############################################################################
## install
## #############################################################################

if(APPLE)
  FixDCMTKMacInstall()
endif()

set_lib_install_rules(${PROJECT_NAME} 
  ${${PROJECT_NAME}_HEADERS}
  )

