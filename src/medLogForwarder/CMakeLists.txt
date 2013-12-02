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

project(medLogForwarder)

## #############################################################################
## find requireds
## #############################################################################

# find dtk
find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

# find itk
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# find vtk
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


## #############################################################################
## List Sources
## #############################################################################

file(GLOB_RECURSE ${PROJECT_NAME}_H
  *.h
  )

file(GLOB_RECURSE ${PROJECT_NAME}_CPP
  *.cpp
  )


## #############################################################################
## include directorie.
## #############################################################################

##  see cmake/module/list_header_directories_to_include.cmake
list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})
    
    
## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_H}
  ${${PROJECT_NAME}_CPP}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore 
  dtkLog
  ITKCommon
  vtkCommon
  )

## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )
