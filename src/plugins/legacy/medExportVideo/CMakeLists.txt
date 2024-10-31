################################################################################
#
# medInria

# Copyright (c) INRIA. All rights reserved.

# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

set(TARGET_NAME ExportVideoPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${${PROJECT_NAME}_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #############################################################################
## Resolve dependencies
## #############################################################################

if(${USE_FFmpeg})
  set (VTK_FFmpeg_MODULE IOFFMPEG)
  set (VTK_FFmpeg_MODULE_LIB VTK::IOFFMPEG)
  add_definitions(-DMED_USE_FFmpeg)
endif()

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(ITK REQUIRED COMPONENTS ITKCommon)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED COMPONENTS CommonCore ImagingSources ${VTK_FFmpeg_MODULE} IOImage RenderingQt IOMovie)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
    ${${TARGET_NAME}_HEADERS}
    )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
    ${MEDINRIA_INCLUDE_DIRS}
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
    ${QT_LIBRARIES}
    ${ITK_LIBRARIES}
    medCore
    medVtkInria
    medUtilities
    ${VTK_LIBRARIES}
    ${VTK_FFmpeg_MODULE_LIB}
    )

vtk_module_autoinit(TARGETS ${TARGET_NAME} MODULES ${VTK_LIBRARIES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
