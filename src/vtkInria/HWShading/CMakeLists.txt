################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

project(vtkHWShading)


## #############################################################################
## List Sources
## #############################################################################

include(cmake/shader_to_header.cmake)


## #############################################################################
## subdir
## #############################################################################

add_subdirectory(helpers)


## #############################################################################
## List Sources
## #############################################################################

file(GLOB ${PROJECT_NAME}_H
  *.h
  )

file(GLOB ${PROJECT_NAME}_CXX
  *.cxx
  )
  
file(GLOB ${PROJECT_NAME}_SHADER
  shaders/*  
  )
 
  
## #############################################################################
## write header shader
## #############################################################################

# see cmake/shader_to_header.cmake
shader_to_header(shaders/AnisotropicLighting.vert
  AnisotropicLightingVertexText
  )

shader_to_header(shaders/AnisotropicLighting.frag
  AnisotropicLightingFragmentText
  )

shader_to_header(shaders/ShadowMapping.vert
  ShadowMappingVertexText
  )
  
shader_to_header(shaders/ShadowMapping.frag
  ShadowMappingFragmentText
  )
  
shader_to_header(shaders/Spotlight.func
  SpotlightFunctionsText
  )
  
shader_to_header(shaders/AnisotropicLighting.func
  AnisotropicLightingFunctionsText
  )
  
shader_to_header(shaders/AnisoLiShadowMap.frag
  AnisotropicLightingShadowMappingFragmentText
  )
  
shader_to_header(shaders/AnisoLiShadowMap.vert
  AnisotropicLightingShadowMappingVertexText
  )
  
shader_to_header(shaders/BuildShadowMap.vert
  BuildShadowMapVertexText
  )
  
shader_to_header(shaders/BuildShadowMap.frag
  BuildShadowMapFragmentText
  )
  
shader_to_header(shaders/BuildShadowMapLines.vert
  BuildShadowMapLinesVertexText
  )
  
shader_to_header(shaders/BuildShadowMapLines.frag
  BuildShadowMapLinesFragmentText
  )

#and include them
include_directories(${${PROJECT_NAME}_BINARY_DIR}/shaders) 

## #############################################################################
## find requireds
## #############################################################################

find_package(OpenGL REQUIRED)


## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${VTKINRIAPCH_H}
    ${VTKINRIAPCH_CPP}
    ${${PROJECT_NAME}CXX}
    )
endif()


## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_PCH}
  ${${PROJECT_NAME}_CXX}
  ${${PROJECT_NAME}_H}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME} 
  ${OPENGL_LIBRARIES}
  vtkRendering
  vtkHelpers
  )


## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )
