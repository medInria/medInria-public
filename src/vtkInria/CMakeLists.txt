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

cmake_minimum_required(VERSION 2.8.9)

project(vtkInria)

## #############################################################################
## Add in the option for building examples, default to ON
## #############################################################################

option(VTKINRIA_BUILD_EXAMPLES 
  "Build vtkInria examples." 
  OFF
  )


## #############################################################################
## Find requireds
## #############################################################################

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

if (APPLE OR WIN32)
    find_package( Boost REQUIRED)
endif()


## #############################################################################
## include directories.
## #############################################################################

file(GLOB_RECURSE ${PROJECT_NAME}_H
  *.h
  )

##  see cmake/module/list_header_directories_to_include.cmake
list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})


## #############################################################################
## Embed vtkInriaPch
## #############################################################################

file(GLOB VTKINRIAPCH_H
  vtkInriaPch/*.h
  )
  
file(GLOB VTKINRIAPCH_CPP
  vtkInriaPch/*.cpp
  )


## #############################################################################
## check environment
## #############################################################################

# For now we always compile all shared
set(${PROJECT_NAME}_BUILD_SHARED_LIBS 1)

if (NOT VTK_USE_GLSL_SHADERS )
    message(FATAL_ERROR " VTK must be compiled with VTK_USE_GLSL_SHADERS at ON 
to use the HWShading library. This option is only available with VTK>=5.1."
    )
endif()

find_path(${PROJECT_NAME}_DATA_DIR 
  NAMES SHTest.vtk 
  DOC "vtkInria Data directory"
  )


## #############################################################################
## add_subdirectory
## #############################################################################

add_subdirectory(vtkDataManagement)
add_subdirectory(vtkImageView)
add_subdirectory(vtkRenderingAddOn)
add_subdirectory(vtkWidgetsAddOn)
add_subdirectory(vtkVisuManagement)
add_subdirectory(HWShading)

if (VTKINRIA_BUILD_EXAMPLES)
  add_subdirectory(Examples)
endif()

