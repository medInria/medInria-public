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

project(vtkItk)


## #############################################################################
## List Sources
## #############################################################################

file(GLOB ${PROJECT_NAME}_H
  *.h
  )

file(GLOB ${PROJECT_NAME}_CXX
  *.cxx
  )
  
file(GLOB ${PROJECT_NAME}_TXX
  *.txx
  )


## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_CXX}
  ${${PROJECT_NAME}_TXX}
  ${${PROJECT_NAME}_H}
  )
  

## #############################################################################
## Link
## #############################################################################
  
target_link_libraries(${PROJECT_NAME}  
  ITKCommon
  VTKCommon
  vtkVisuManagement
  )

## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_lib_install_rules(${PROJECT_NAME} 
  ${${PROJECT_NAME}_H}
  )
