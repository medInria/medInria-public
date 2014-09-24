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
if (NOT VTK_USE_GLSL_SHADERS )
    message(FATAL_ERROR " VTK must be compiled with VTK_USE_GLSL_SHADERS at ON 
to use the HWShading library. This option is only available with VTK>=5.1."
    )
endif()

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
  medClutEditorToolBox
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
  ${Boost_INCLUDE_DIR}
  ${medCore_INCLUDE_DIRS}
  ${${PROJECT_NAME}_BINARY_DIR}/shaders
  )


## #############################################################################
## Embed vtkInriaPch
## #############################################################################

if(${PROJECT_NAME}_USE_PRECOMPILED_HEADERS)
  file(GLOB VTKINRIAPCH_H
    vtkInriaPch/*.h
    )
  
  file(GLOB VTKINRIAPCH_CPP
    vtkInriaPch/*.cpp
    )
endif()


## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${VTKINRIAPCH_H}
    ${VTKINRIAPCH_CPP}
    ${${PROJECT_NAME}_SOURCES}
    )
endif()


## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_CFILES}
  ${${PROJECT_NAME}_PCH}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}  
  ${OPENGL_LIBRARIES}
  medCore
  vtkCommon
  vtkFiltering 
  vtkRendering
  vtkWidgets
  vtkCommon
  vtkFiltering 
  vtkRendering
  vtkWidgets
  vtkHybrid
  vtkImaging
  vtkVolumeRendering
  vtkGraphics
  vtkInfovis
  vtkIO
  ITKVTK
  ITKCommon
  ITKIOPhilipsREC # ITK4 IO libs
  ITKIOBMP
  ITKIOBioRad
  ITKIOHDF5
  ITKIOGDCM
  ITKIOGIPL
  ITKIOGE
  ITKIOJPEG
  ITKIOLSM
  ITKIOMeta
  ITKIONIFTI
  ITKIONRRD
  ITKIOPNG
  ITKIOStimulate
  ITKIOVTK
  ITKIOMRC
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
