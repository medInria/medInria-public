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

project(undoRedoRegistrationPlugin)

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

find_package(ITK REQUIRED)
if(ITK_FOUND)
   include(${ITK_USE_FILE})
endif(ITK_FOUND)

find_package(RPI REQUIRED)
include(${RPI_USE_FILE})

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  undoRedoRegistrationPlugin.h
  undoRedoRegistrationToolBox.h
  undoRedoRegistration.h)

set(${PROJECT_NAME}_SOURCES
  undoRedoRegistrationPlugin.cpp
  undoRedoRegistrationToolBox.cpp
  undoRedoRegistration.cpp
  )

set(${PROJECT_NAME}_RESOURCES undoRedoRegistration.qrc)

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})
qt4_add_RESOURCES(${PROJECT_NAME}_SOURCES_QRC ${${PROJECT_NAME}_RESOURCES})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_SOURCES_QRC}
  )

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkLog
  dtkCore
  medCore
  medGui
  itkProcessRegistration
  RegistrationAddOn
  registrationFactory
)

## #################################################################
## Source file layout in development environments like Visual Studio
## #################################################################

SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})



## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib)
