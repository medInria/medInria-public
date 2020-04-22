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

set(TARGET_NAME vtkDataMeshPlugin)

## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(dtk REQUIRED)

find_package(VTK REQUIRED COMPONENTS vtkCommonCore vtkCommonDataModel vtkRenderingCore
    vtkRenderingVolumeOpenGL2 vtkInteractionStyle vtkInteractionWidgets vtkIOParallelXML
    vtkIOImage vtkIOGeometry  vtkIOExport vtkRenderingContext2D
    vtkIOXML vtkIOCore vtkIOLegacy)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED COMPONENTS ITKCommon)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  .
  datas
  interactors
  navigators
  readers
  writers
  )

## #############################################################################
## Precompile headers
## #############################################################################

if (MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${TARGET_NAME}_PCH
    ${MEDPLUGINSPCH_H}
    ${MEDPLUGINSPCH_CPP}
    ${${TARGET_NAME}_SOURCES}
    )
endif()

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_PCH}
  ${${TARGET_NAME}_CFILES}
  )

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${${PROJECT_NAME}_VERSION})
string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
target_compile_definitions(${TARGET_NAME} PUBLIC ${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

target_include_directories(${TARGET_NAME}
  PUBLIC ${${TARGET_NAME}_INCLUDE_DIRS}
  PUBLIC ${dtk_INCLUDE_DIRS}
  PRIVATE ${ITK_INCLUDE_DIRS}
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES}
  medCore
  medLog
  medVtkInria
  medVtkDataMeshBase
  )

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
