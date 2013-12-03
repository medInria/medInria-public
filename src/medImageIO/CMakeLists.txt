################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
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

file(GLOB HEADERS
  *.h
  )
set(${PROJECT_NAME}_H
  ${HEADERS}
  ${${PROJECT_NAME}_H}
  )

file(GLOB CPP
  *.cpp
  )
set(${PROJECT_NAME}_CPP
  ${CPP}
  ${${PROJECT_NAME}_CPP}
  )

  
## #############################################################################
## include directorie.
## #############################################################################

if(APPLE)
  include(FixDCMTKMacLink)
  FixDCMTKMacLink(${PROJECT_NAME})
endif(APPLE)

##  see cmake/module/list_header_directories_to_include.cmake
list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${DCMTK_INCLUDE_DIRS}
  ${medCore_INCLUDE_DIRS}
  )


## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_CPP}
  ${${PROJECT_NAME}_H}
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
  ITKIOHDF5
  ITKIOBioRad
  ITKIOStimulate
  ${ITKReview_LIBRARIES} #TODO get rid of this ASAP.
  ${DCMTK_LIBRARIES}
  )


## #############################################################################
## install
## #############################################################################

if(APPLE)
  include(FixDCMTKMacInstall)
  FixDCMTKMacInstall()
endif()

## see cmake/module/set_lib_install_rules.Cmake
set_lib_install_rules(${PROJECT_NAME} 
  ${${PROJECT_NAME}_H}
  )

