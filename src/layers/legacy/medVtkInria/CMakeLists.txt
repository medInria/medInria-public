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

set(TARGET_NAME medVtkInria)

## #############################################################################
## Find required
## #############################################################################

# find dtk
find_package(dtk REQUIRED)

find_package(VTK REQUIRED COMPONENTS RenderingVolumeOpenGL2 InteractionWidgets RenderingAnnotation 
  InteractionStyle ImagingStatistics IOLegacy)

find_package(OpenGL REQUIRED)

if (USE_OSPRay)
  add_definitions(-DMED_USE_OSPRAY_4_VR_BY_CPU)
  set(VTK_OSPRAY_RENDERING_LIBRARY "vtkRenderingOSPRay")
ELSE()
  set(VTK_OSPRAY_RENDERING_LIBRARY "")
endif()

## #############################################################################
## List sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  vtkImageView
  vtkWidgetsAddOn
  vtkVisuManagement
  vtkVisuManagement/lut
  medClutEditorToolBox
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
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PRIVATE ${dtk_INCLUDE_DIRS}
  PUBLIC
	"$<BUILD_INTERFACE:${${TARGET_NAME}_INCLUDE_DIRS}>"
	"$<INSTALL_INTERFACE:include/${TARGET_NAME}>"
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME} PUBLIC
  ${OPENGL_LIBRARIES}
  Qt5::Core
  Qt5::Widgets
  
  med::Core
  
  VTK::CommonCore
  VTK::CommonSystem
  VTK::ImagingColor
  VTK::ImagingGeneral
  VTK::ImagingHybrid
  VTK::IOCore
  VTK::IOGeometry
  VTK::IOImage
  VTK::IOLegacy
  VTK::IOXML
  VTK::RenderingAnnotation
  VTK::RenderingCore
  VTK::RenderingOpenGL2
  VTK::RenderingVolumeOpenGL2
  ${VTK_OSPRAY_RENDERING_LIBRARY}
  VTK::InteractionWidgets
  VTK::InteractionStyle
  VTK::FiltersExtraction
  VTK::FiltersModeling
  VTK::ImagingStencil
  VTK::FiltersGeometry
  VTK::vtksys
  
  
  # dtk
  # Legacy
  dtkCoreSupport
  )

vtk_module_autoinit(TARGETS ${TARGET_NAME} MODULES ${VTK_LIBRARIES})

## #############################################################################
## install
## #############################################################################

# set_lib_install_rules(${TARGET_NAME}
#   ${${TARGET_NAME}_HEADERS}
#   )


set_lib_install_rules(${TARGET_NAME}
    NAMESPACE                  med
    VERSION                    4.0    
    LIBRARY_OUTPUT_NAME_PREFIX med
    DESTINATION                ${MEDINRIA_PATH}
    
    PUBLIC_DEPENDENCIES      
      "medInria REQUIRED COMPONENTS Core"  
      "dtk      REQUIRED"
      "VTK      REQUIRED COMPONENTS vtkRenderingVolumeOpenGL2 vtkInteractionWidgets vtkRenderingAnnotation vtkInteractionStyle vtkImagingStatistics vtkIOLegacy"
      "OpenGL   REQUIRED"
    )
