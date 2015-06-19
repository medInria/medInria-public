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

cmake_minimum_required(VERSION 2.8.9)

project(medVtkInria)

## #############################################################################
## Find required
## #############################################################################

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

if (APPLE OR WIN32)
    find_package(Boost REQUIRED)
endif()

find_package(OpenGL REQUIRED)


## #############################################################################
## List sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  vtkDataManagement
  vtkImageView
  vtkWidgetsAddOn
  vtkVisuManagement
  vtkVisuManagement/lut
  vtkItkWrap
  HWShading
  HWShading/helpers
  )


## #############################################################################
## write header shader
## #############################################################################


include(HWShading/cmake/shader_to_header.cmake)

# see HWShading/cmake/shader_to_header.cmake
shader_to_header(HWShading/shaders/AnisotropicLighting.vert
  AnisotropicLightingVertexText
  )

shader_to_header(HWShading/shaders/AnisotropicLighting.frag
  AnisotropicLightingFragmentText
  )

shader_to_header(HWShading/shaders/ShadowMapping.vert
  ShadowMappingVertexText
  )
  
shader_to_header(HWShading/shaders/ShadowMapping.frag
  ShadowMappingFragmentText
  )
  
shader_to_header(HWShading/shaders/Spotlight.func
  SpotlightFunctionsText
  )
  
shader_to_header(HWShading/shaders/AnisotropicLighting.func
  AnisotropicLightingFunctionsText
  )
  
shader_to_header(HWShading/shaders/AnisoLiShadowMap.frag
  AnisotropicLightingShadowMappingFragmentText
  )
  
shader_to_header(HWShading/shaders/AnisoLiShadowMap.vert
  AnisotropicLightingShadowMappingVertexText
  )
  
shader_to_header(HWShading/shaders/BuildShadowMap.vert
  BuildShadowMapVertexText
  )
  
shader_to_header(HWShading/shaders/BuildShadowMap.frag
  BuildShadowMapFragmentText
  )
  
shader_to_header(HWShading/shaders/BuildShadowMapLines.vert
  BuildShadowMapLinesVertexText
  )
  
shader_to_header(HWShading/shaders/BuildShadowMapLines.frag
  BuildShadowMapLinesFragmentText
  )
  

## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR}
  ${medCore_INCLUDE_DIRS}
  ${${PROJECT_NAME}_BINARY_DIR}/shaders
  )

## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_CFILES})

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}  
  ${OPENGL_LIBRARIES}
  ITKIOImageBase
  ITKIOBMP
  ITKIOLSM
  ITKIOJPEG
  ITKIOPNG
  ITKIOVTK
  ITKIOGDCM
  ITKIOGE
  ITKNrrdIO
  ITKIONRRD
  ITKIOMeta
  ITKIONIFTI
  ITKIOGIPL
  ITKIOHDF5
  ITKIOBioRad
  ITKIOStimulate
  ${ITKIOPhilipsREC_LIBRARIES}
  ITKIOMRC
  ${ITKReview_LIBRARIES} #TODO Get rid of this ASAP
  vtkCommonCore
  vtkIOCore
  vtkIOLegacy
  vtkIOXML
  vtkRenderingCore
  vtkRenderingOpenGL
  vtkRenderingVolumeOpenGL
  #vtkRenderingVolumeOpenGLNew
  vtkRenderingFreeTypeOpenGL
  vtkRenderingContextOpenGL
  vtkInteractionWidgets
  vtkInteractionStyle
  vtkFiltersExtraction
  vtkImagingStencil
  vtkFiltersGeometry
  vtksys
  )


## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

## #############################################################################
## Build tests
## #############################################################################

if(MEDINRIA_BUILD_TESTS)
  add_subdirectory(tests)
endif()
