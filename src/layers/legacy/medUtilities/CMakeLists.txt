################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

set(TARGET_NAME medUtilities)

## #############################################################################
## Find required
## #############################################################################

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(VTK REQUIRED COMPONENTS vtkRenderingVolumeOpenGL2 vtkInteractionWidgets vtkRenderingAnnotation vtkInteractionStyle vtkImagingStatistics vtkIOLegacy )
include(${VTK_USE_FILE})

find_package(ITK REQUIRED COMPONENTS ITKCommon)
include(${ITK_USE_FILE})

## #############################################################################
## List sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
${${TARGET_NAME}_CFILES})

## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PUBLIC
  ${${TARGET_NAME}_INCLUDE_DIRS}
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ${QT_LIBRARIES}
  ${ITK_LIBRARIES}
  dtkCore
  dtkLog
  ${OPENGL_LIBRARIES}
  medCore
  medCoreLegacy
  medVtkInria
  medVtkDataMeshBase
  ITKCommon
  vtkCommonCore
  vtkCommonSystem
  vtkImagingColor
  vtkImagingGeneral
  vtkImagingHybrid
  vtkIOCore
  vtkIOExportOpenGL2
  vtkIOGeometry
  vtkIOImage
  vtkIOLegacy
  vtkIOXML
  vtkRenderingAnnotation
  vtkRenderingCore
  vtkRenderingOpenGL2
  vtkRenderingVolumeOpenGL2
  vtkRenderingContextOpenGL2
  ${VTK_OSPRAY_RENDERING_LIBRARY}
  vtkInteractionWidgets
  vtkInteractionStyle
  vtkFiltersExtraction
  vtkFiltersModeling
  vtkImagingStencil
  vtkFiltersGeometry
  vtksys
  itksys
  )


## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )
