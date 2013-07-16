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

project(exampleProcessImageFilterPlugin)

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
include(${medInria_USE_FILE})
include(dtkPlugin)

find_package(ITK REQUIRED)
  if (NOT ITK_USE_REVIEW)
    message(FATAL_ERROR "ITK must be build with option ITK_USE_REVIEW to ON")
  endif()
  if (NOT ITK_USE_REVIEW_STATISTICS)
    message(FATAL_ERROR "ITK must be build with option ITK_USE_REVIEW_STATISTICS to ON")
  endif()
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

include_directories(../dtkItkLogForwarder)
include_directories(../v3dView)


## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  exampleProcessImageFilterPlugin.h
  exampleProcessImageFilter.h
  exampleProcessImageFilterToolBox.h)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  exampleProcessImageFilterPlugin.cpp
  exampleProcessImageFilter.cpp
  exampleProcessImageFilterToolBox.cpp)

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_MOC} 
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_HEADERS})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore dtkLog
  medCore
  medGui
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES})

dtk_build_doc(exampleProcessImageFilterPlugin)

## #################################################################
## Source file layout in development environments like Visual Studio
## #################################################################

source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION plugins)

