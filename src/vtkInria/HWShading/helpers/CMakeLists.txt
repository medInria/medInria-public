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


## #############################################################################
## List Sources
## #############################################################################

file(GLOB vtkHelpers_H
  *.h
  )

file(GLOB vtkHelpers_CXX
  *.cxx
  )


## #############################################################################
## add library
## #############################################################################

add_library(vtkHelpers SHARED 
  ${vtkHelpers_H}
  ${vtkHelpers_CXX}

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
  ${vtkHelpers_H}
  ) 
