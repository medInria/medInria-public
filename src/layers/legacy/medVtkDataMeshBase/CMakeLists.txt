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

find_package(dtk REQUIRED)
#include_directories(${dtk_INCLUDE_DIRS})

find_package(VTK REQUIRED COMPONENTS CommonCore CommonDataModel RenderingCore RenderingVolumeOpenGL2 
  InteractionStyle InteractionWidgets IOParallelXML IOImage IOGeometry IOExport RenderingContext2D IOInfovis)

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

add_library(${TARGET_NAME} SHARED)
target_sources(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_CFILES} ${${TARGET_NAME}_QRC} )
target_sources(${TARGET_NAME} PRIVATE ${RELATIVE_${TARGET_NAME}_HEADERS})
set_target_properties(${TARGET_NAME} PROPERTIES PUBLIC_HEADER "${RELATIVE_${TARGET_NAME}_HEADERS}")

## #############################################################################
## include directories
## #############################################################################

target_include_directories(${TARGET_NAME}
  PUBLIC
	"$<BUILD_INTERFACE:${${TARGET_NAME}_INCLUDE_DIRS}>"
	"$<INSTALL_INTERFACE:include/${TARGET_NAME}>"
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME} PUBLIC
  ${VTK_LIBRARIES}
  med::Core
  med::Log
  med::VtkInria
  )

vtk_module_autoinit(TARGETS ${TARGET_NAME} MODULES ${VTK_LIBRARIES})

## #############################################################################
## install
## #############################################################################

#set_lib_install_rules(${TARGET_NAME}
#  ${${TARGET_NAME}_HEADERS}
#  )

set_lib_install_rules(${TARGET_NAME}
    NAMESPACE                  med
    VERSION                    4.0    
    LIBRARY_OUTPUT_NAME_PREFIX med
    DESTINATION                ${MEDINRIA_PATH}
    
    PUBLIC_DEPENDENCIES      
      "medInria REQUIRED COMPONENTS Core Log VtkInria"
      "dtk REQUIRED"
      "VTK REQUIRED COMPONENTS vtkCommonCore vtkCommonDataModel vtkRenderingCore vtkRenderingVolumeOpenGL2 vtkInteractionStyle vtkInteractionWidgets vtkIOParallelXML vtkIOImage vtkIOGeometry  vtkIOExport vtkRenderingContext2D vtkIOInfovis"
      "ITK REQUIRED COMPONENTS ITKCommon"
    )

## #############################################################################
## Build tests
## #############################################################################

if(${PROJECT_NAME}_BUILD_TESTS)
  add_subdirectory(tests)
endif()
