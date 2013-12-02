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

project(vtkVisuManagement)

if (CMAKE_CXX_COMPILER MATCHES clang)
    add_definitions(-DCLANG)
endif()

## #############################################################################
## List Sources
## #############################################################################

file(GLOB ${PROJECT_NAME}_H
  *.h
  )

file(GLOB ${PROJECT_NAME}_CXX
  *.cxx
  )

file(GLOB vtkLandmarkManagement_CXX
  vtkLandmarkManagement/*.cxx
  )

file(GLOB vtkLandmarkManagement_H
  vtkLandmarkManagement/*.h
  )

set(${PROJECT_NAME}_LIBS
 vtkCommon
 vtkFiltering 
 vtkRendering
 vtkWidgets
 vtkDataManagement
 vtkHWShading
)

set(vtkLandmarkManagement_LIBS
 vtkCommon
 vtkFiltering
 vtkRendering
 vtkWidgets
 vtkDataManagement
 vtkImageView
 vtkHWShading
)


## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${VTKINRIAPCH_H}
    ${VTKINRIAPCH_CPP}
    ${${PROJECT_NAME}_CPP}
    )
endif()


## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_PCH}
  ${${PROJECT_NAME}_CXX}
  ${${PROJECT_NAME}_H}
  )
  
add_library(vtkLandmarkManagement SHARED
  ${vtkLandmarkManagement_CXX}
  ${vtkLandmarkManagement_H}
  )


## #############################################################################
## Link
## #############################################################################
  
target_link_libraries(${PROJECT_NAME}  
  ${${PROJECT_NAME}_LIBS}
  )

target_link_libraries(vtkLandmarkManagement  
  ${vtkLandmarkManagement_LIBS}
  )


## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )
  
set_lib_install_rules(vtkLandmarkManagement
  ${vtkLandmarkManagement_H}
  )
