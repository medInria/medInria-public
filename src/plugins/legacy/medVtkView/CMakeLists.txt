################################################################################
#
# medInria

# Copyright (c) INRIA 2013. All rights reserved.

# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

set(TARGET_NAME medVtkViewPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${${PROJECT_NAME}_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")


## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(VTK REQUIRED COMPONENTS vtkGUISupportQt vtkInteractionWidgets)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED COMPONENTS ITKCommon)
include(${ITK_USE_FILE})

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

set(${TARGET_NAME}_QRC resources/medVtkView.qrc)

## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${${TARGET_NAME}_INCLUDE_DIRS}
  )

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  Qt5::OpenGL
  dtkCore
  dtkLog
  medCore
  medVtkInria
  vtkGUISupportQt
  vtkGUISupportQtOpenGL
  medVtkDataMeshBase
  )


## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})


## #############################################################################
## Build tests
## #############################################################################

if(${TARGET_NAME}_BUILD_TESTS)
  #add_subdirectory(tests)
endif()

