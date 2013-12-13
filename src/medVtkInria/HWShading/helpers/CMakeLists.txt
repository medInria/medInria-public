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

project(vtkHelpers)

## #############################################################################
## List Sources
## #############################################################################

file(GLOB ${PROJECT_NAME}_H
  *.h
  )

file(GLOB ${PROJECT_NAME}_CXX
  *.cxx
  )


## #############################################################################
## add library
## #############################################################################

add_library(vtkHelpers SHARED 
  ${${PROJECT_NAME}_H}
  ${${PROJECT_NAME}_CXX}

  )

## #############################################################################
## link
## #############################################################################

target_link_libraries(vtkHelpers
  vtkCommon
  vtkIO
  vtkFiltering
  vtkImaging
  vtkGraphics
  vtkRendering
  )


## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_lib_install_rules(vtkHelpers
  ${${PROJECT_NAME}_H}
  ) 
