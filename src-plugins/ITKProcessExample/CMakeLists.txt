#######################################################################
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
#######################################################################

cmake_minimum_required(VERSION 2.8.1)

project(ITKProcessExamplePlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION ${medInria_VERSION})

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

# ITK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  ITKProcessExamplePlugin.h
  ITKProcessExampleToolBox.h
  ITKProcessExample.h)

set(${PROJECT_NAME}_SOURCES
  ITKProcessExamplePlugin.cpp
  ITKProcessExampleToolBox.cpp
  ITKProcessExample.cpp)

## #################################################################
## Build rules
## #################################################################

configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/ITKProcessExampleDefinitions.h.in
      ${CMAKE_CURRENT_BINARY_DIR}/ITKProcessExampleDefinitions.h
      )

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore dtkLog
  medCore
  medGui
  ITKCommon
)

#dtk_build_doc(ITKProcessExamplePlugin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_executable(${PROJECT_NAME}Testing
  ITKProcessExampleTestingApp.cpp
)

target_link_libraries(${PROJECT_NAME}Testing
  ${PROJECT_NAME}
)

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib)
