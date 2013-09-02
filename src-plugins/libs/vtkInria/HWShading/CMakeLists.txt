#######################################################################
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
#######################################################################

project(vtkHWShading)

include(ShaderText.cmake)

set(${PROJECT_NAME}_classes
  ExtensionInitialize
  vtkShaderBase
  vtkShaderBaseHandle
  vtkShaderObject
  vtkVertexShader
  vtkFragmentShader
  vtkShaderObjectCollection
  vtkShaderUniform
  vtkUniformFloat
  vtkUniformVec2
  vtkUniformVec3
  vtkUniformVec4
  vtkUniformBool
  vtkUniformInt
  vtkUniformSampler
  vtkShaderUniformCollection
  vtkBMIAShaderProgram
  vtkBMIAShaderProgramReader
  vtkAnisotropicLightingSP
  vtkShadowMappingSP
  vtkShadowMappingHelper
  vtkShadowRenderer
  vtkAnisoLiShadowMapSP
  vtkShadowMappingHelperLines
  vtkFiberMapper
  )


foreach (class ${${PROJECT_NAME}_classes})
  set(${PROJECT_NAME}_SRCS    ${${PROJECT_NAME}_SRCS}    ${class}.cxx)
  set(${PROJECT_NAME}_HEADERS ${${PROJECT_NAME}_HEADERS} ${class}.h)
endforeach()


shader_to_header("AnisotropicLightingVertexText"
	   "Shaders/AnisotropicLighting.vert"
     "${vtkInria_BINARY_DIR}/Shaders/AnisotropicLightingVertexText.h"
	  )
shader_to_header("AnisotropicLightingFragmentText"
	   "Shaders/AnisotropicLighting.frag"
     "${vtkInria_BINARY_DIR}/Shaders/AnisotropicLightingFragmentText.h"
	  )
shader_to_header("ShadowMappingVertexText"
	   "Shaders/ShadowMapping.vert"
	   "${vtkInria_BINARY_DIR}/Shaders/ShadowMappingVertexText.h"
	  )
shader_to_header("ShadowMappingFragmentText"
	   "Shaders/ShadowMapping.frag"
	   "${vtkInria_BINARY_DIR}/Shaders/ShadowMappingFragmentText.h"
	  )
shader_to_header("SpotlightFunctionsText"
	   "Shaders/Spotlight.func"
	   "${vtkInria_BINARY_DIR}/Shaders/SpotlightFunctionsText.h"
	  )
shader_to_header("AnisotropicLightingFunctionsText"
	   "Shaders/AnisotropicLighting.func"
	   "${vtkInria_BINARY_DIR}/Shaders/AnisotropicLightingFunctionsText.h"
	  )
shader_to_header("AnisotropicLightingShadowMappingFragmentText"
	   "Shaders/AnisoLiShadowMap.frag"
	   "${vtkInria_BINARY_DIR}/Shaders/AnisotropicLightingShadowMappingFragmentText.h"
	  )
shader_to_header("AnisotropicLightingShadowMappingVertexText"
	   "Shaders/AnisoLiShadowMap.vert"
	   "${vtkInria_BINARY_DIR}/Shaders/AnisotropicLightingShadowMappingVertexText.h"
	  )
shader_to_header("BuildShadowMapVertexText"
	   "Shaders/BuildShadowMap.vert"
	   "${vtkInria_BINARY_DIR}/Shaders/BuildShadowMapVertexText.h"
	  )
shader_to_header("BuildShadowMapFragmentText"
	   "Shaders/BuildShadowMap.frag"
	   "${vtkInria_BINARY_DIR}/Shaders/BuildShadowMapFragmentText.h"
	  )
shader_to_header("BuildShadowMapLinesVertexText"
	   "Shaders/BuildShadowMapLines.vert"
	   "${vtkInria_BINARY_DIR}/Shaders/BuildShadowMapLinesVertexText.h"
	  )
shader_to_header("BuildShadowMapLinesFragmentText"
	   "Shaders/BuildShadowMapLines.frag"
	   "${vtkInria_BINARY_DIR}/Shaders/BuildShadowMapLinesFragmentText.h"
	  )

find_package(OpenGL REQUIRED)

set(${PROJECT_NAME}_SOURCES_PCH)
if (VTKINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH
    "vtkInriaPch/vtkInriaPch.h" "../vtkInriaPch/vtkInriaPch.cpp"
    ${${PROJECT_NAME}_SRCS}
  )
endif()

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SRCS}
  ${${PROJECT_NAME}_HEADERS}
)

target_link_libraries(${PROJECT_NAME} 
  vtkRendering
  ${OPENGL_LIBRARIES}
  )

if (VTKINRIA_BUILD_TESTING)
    add_subdirectory(Testing)
endif()


## #################################################################
## Install rules
## #################################################################
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME}) 
