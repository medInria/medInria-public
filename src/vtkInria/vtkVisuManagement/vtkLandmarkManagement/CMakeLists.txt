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

project(vtkLandmarkManagement)

if (CMAKE_CXX_COMPILER MATCHES clang)
    add_definitions(-DCLANG)
endif()

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

file(GLOB CXX
  *.cxx
  )
set(${PROJECT_NAME}_CXX
  ${CXX}
  ${${PROJECT_NAME}_CXX}
  )

set(${PROJECT_NAME}_LIBS
 vtkCommon
 vtkFiltering
 vtkRendering
 vtkWidgets
 vtkDataManagement
 vtkImageView
 vtkHWShading
)


## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_CXX}
  ${${PROJECT_NAME}_H}
  )


## #############################################################################
## Link
## #############################################################################
  
target_link_libraries(${PROJECT_NAME}  
  ${${PROJECT_NAME}_LIBS}
  )


## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )
