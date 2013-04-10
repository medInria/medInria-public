project(vtkHWShading)

INCLUDE(ShaderText.cmake)

add_subdirectory(Helpers)

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


SHADER_TO_HEADER("AnisotropicLightingVertexText"
	   "Shaders/AnisotropicLighting.vert"
     "${vtkInria_BINARY_DIR}/Shaders/AnisotropicLightingVertexText.h"
	  )
SHADER_TO_HEADER("AnisotropicLightingFragmentText"
	   "Shaders/AnisotropicLighting.frag"
     "${vtkInria_BINARY_DIR}/Shaders/AnisotropicLightingFragmentText.h"
	  )
SHADER_TO_HEADER("ShadowMappingVertexText"
	   "Shaders/ShadowMapping.vert"
	   "${vtkInria_BINARY_DIR}/Shaders/ShadowMappingVertexText.h"
	  )
SHADER_TO_HEADER("ShadowMappingFragmentText"
	   "Shaders/ShadowMapping.frag"
	   "${vtkInria_BINARY_DIR}/Shaders/ShadowMappingFragmentText.h"
	  )
SHADER_TO_HEADER("SpotlightFunctionsText"
	   "Shaders/Spotlight.func"
	   "${vtkInria_BINARY_DIR}/Shaders/SpotlightFunctionsText.h"
	  )
SHADER_TO_HEADER("AnisotropicLightingFunctionsText"
	   "Shaders/AnisotropicLighting.func"
	   "${vtkInria_BINARY_DIR}/Shaders/AnisotropicLightingFunctionsText.h"
	  )
SHADER_TO_HEADER("AnisotropicLightingShadowMappingFragmentText"
	   "Shaders/AnisoLiShadowMap.frag"
	   "${vtkInria_BINARY_DIR}/Shaders/AnisotropicLightingShadowMappingFragmentText.h"
	  )
SHADER_TO_HEADER("AnisotropicLightingShadowMappingVertexText"
	   "Shaders/AnisoLiShadowMap.vert"
	   "${vtkInria_BINARY_DIR}/Shaders/AnisotropicLightingShadowMappingVertexText.h"
	  )
SHADER_TO_HEADER("BuildShadowMapVertexText"
	   "Shaders/BuildShadowMap.vert"
	   "${vtkInria_BINARY_DIR}/Shaders/BuildShadowMapVertexText.h"
	  )
SHADER_TO_HEADER("BuildShadowMapFragmentText"
	   "Shaders/BuildShadowMap.frag"
	   "${vtkInria_BINARY_DIR}/Shaders/BuildShadowMapFragmentText.h"
	  )
SHADER_TO_HEADER("BuildShadowMapLinesVertexText"
	   "Shaders/BuildShadowMapLines.vert"
	   "${vtkInria_BINARY_DIR}/Shaders/BuildShadowMapLinesVertexText.h"
	  )
SHADER_TO_HEADER("BuildShadowMapLinesFragmentText"
	   "Shaders/BuildShadowMapLines.frag"
	   "${vtkInria_BINARY_DIR}/Shaders/BuildShadowMapLinesFragmentText.h"
	  )

FIND_PACKAGE(OpenGL REQUIRED)

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
  vtkHelpers
  ${OPENGL_LIBRARIES}
  )

if ( VTKINRIA_BUILD_TESTING )
  add_subdirectory( Testing )
endif()
