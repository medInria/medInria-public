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

set(TARGET_NAME ITKProcessExamplePlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

# ITK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

## #################################################################
## List Sources
## #################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
  
  
## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  )


## #################################################################
## Build rules
## #################################################################

configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/ITKProcessExampleDefinitions.h.in
      ${CMAKE_CURRENT_BINARY_DIR}/ITKProcessExampleDefinitions.h
      )


## #################################################################
## add library
## #################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES})
  
  
## #############################################################################
## Link
## #############################################################################


target_link_libraries(${TARGET_NAME}
  ${QT_LIBRARIES}
  dtkCore
  dtkLog
  ITKCommon
  medCore
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${TARGET_NAME}Testing
  ITKProcessExampleTestingApp.cpp
)

target_link_libraries(${TARGET_NAME}Testing
  ${TARGET_NAME}
)

## #################################################################
## Install rules
## #################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})

