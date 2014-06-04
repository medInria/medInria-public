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

project(cliSupportWorkspacePlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${PROJECT_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")

## #################################################################
## Resolve dependencies
## #################################################################

find_package(dtk REQUIRED)

include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(CTK REQUIRED)
include(${CTK_USE_FILE})

# Add a define to set the path to the designer plugins in the source
add_definitions(-DCTK_QTDESIGNERPLUGINS_DIR="${CTK_QTDESIGNERPLUGINS_DIR}")

set(QT_USE_QTUITOOLS 1)
include(${QT_USE_FILE})

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  . # current dir
  )

## #############################################################################
## include directories
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

ctkFunctionGetIncludeDirs(CTK_includes ${PROJECT_NAME})

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  ${CTK_includes}
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
# Finds the listed libraries in target_libraries.cmake
ctkFunctionGetTargetLibraries(CTK_target_libraries)

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkLog
  dtkCore
  medCore
  ${CTK_target_libraries}
  )

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules(${PROJECT_NAME})

