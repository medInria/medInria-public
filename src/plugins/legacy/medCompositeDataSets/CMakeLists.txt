################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

cmake_minimum_required(VERSION 2.8.1)

project(medCompositeDataSetsPlugin)

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

find_package(TTK REQUIRED)
include(${TTK_USE_FILE})

#find_package(ITK REQUIRED)
#include(${ITK_USE_FILE})

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  DiffusionSequenceWidget.h
  medCompositeDataSetsPlugin.h
  medCompositeDataSetsReader.h
  medCompositeDataSetsWriter.h
  medDiffusionSequenceCompositeDataToolBox.h
)

set(${PROJECT_NAME}_HEADERS
  medCompositeDataSetsBase.h
  dirTools.h
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  dirTools.cpp
  medCompositeDataSetsPlugin.cpp
  medDiffusionSequenceCompositeDataToolBox.cpp
  medCompositeDataSetsReader.cpp
  medCompositeDataSetsWriter.cpp
  medDiffusionSequenceCompositeData.cpp)

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
  dtkCore 
  dtkLog
  dtkZip
  ITKTensor
  ITKCommon  
  medCore
  medGui
)
#not useful if plugin in the medInria-plugins directory
dtk_build_doc(medCompositeDataSetsPlugin)

#add_executable(test test.C)
#target_link_libraries(test medCompositeDataSetsPlugin ${QT_LIBRARIES} dtkCore dtkLog dtkZip medCore)

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
  ARCHIVE DESTINATION lib)
