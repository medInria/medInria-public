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

cmake_minimum_required(VERSION 2.8.9)

project(medVtkView2)

## #############################################################################
## Find required
## #############################################################################

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


## #############################################################################
## List sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
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
#  vtkCommonCore
#  vtkIOCore
#  vtkIOXML
#  vtkRenderingCore
#  vtkRenderingOpenGL
#  vtkRenderingVolumeOpenGL
#  vtkRenderingFreeTypeOpenGL
#  vtkRenderingContextOpenGL
  ${VTK_LIBRARIES}
  )

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )
