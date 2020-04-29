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

set(TARGET_NAME medVtkDataMeshBase)

## #############################################################################
## Find required
## #############################################################################

find_package(VTK REQUIRED COMPONENTS
    vtkRenderingVolumeOpenGL2  vtkRenderingContext2D
    vtkInteractionWidgets
    vtkIOParallelXML vtkIOGeometry  vtkIOExport
    )

find_package(ITK REQUIRED COMPONENTS ITKCommon)

## #############################################################################
## List sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
   "${${TARGET_NAME}_QRC}"
  )

## #############################################################################
## source
## #############################################################################
target_sources(${TARGET_NAME}
    PRIVATE ${${TARGET_NAME}_SOURCES}
    PUBLIC ${${TARGET_NAME}_TEMPLATES}
    PUBLIC ${${TARGET_NAME}_HEADERS}
)
## #############################################################################
## include directories
## #############################################################################
list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

target_include_directories(${TARGET_NAME}
  PUBLIC ${${TARGET_NAME}_INCLUDE_DIRS}
  PUBLIC ${dtk_INCLUDE_DIRS}
  PRIVATE ${ITK_INCLUDE_DIRS}
  PRIVATE ${VTK_INCLUDE_DIRS}
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ${VTK_LIBRARIES}
  ${ITK_LIBRARIES}
  medCore
  medLog
  medVtkInria
  )

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

## #############################################################################
## Build tests
## #############################################################################

if(${PROJECT_NAME}_BUILD_TESTS)
  add_subdirectory(tests)
endif()
