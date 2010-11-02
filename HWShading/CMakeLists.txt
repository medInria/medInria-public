
  INCLUDE(ShaderText.cmake)

  ADD_SUBDIRECTORY(Helpers)

  SET(HWShading_classes
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


  FOREACH(class ${HWShading_classes})
    SET(HWShading_SRCS 
    ${HWShading_SRCS} ${class}.h ${class}.cxx 
    )
  ENDFOREACH(class)


  SHADER_TO_HEADER("AnisotropicLightingVertexText"
		   "Shaders/AnisotropicLighting.vert"
	           "${vtkINRIA3D_BINARY_DIR}/Shaders/AnisotropicLightingVertexText.h"
		  )
  SHADER_TO_HEADER("AnisotropicLightingFragmentText"
		   "Shaders/AnisotropicLighting.frag"
	           "${vtkINRIA3D_BINARY_DIR}/Shaders/AnisotropicLightingFragmentText.h"
		  )
  SHADER_TO_HEADER("ShadowMappingVertexText"
		   "Shaders/ShadowMapping.vert"
		   "${vtkINRIA3D_BINARY_DIR}/Shaders/ShadowMappingVertexText.h"
		  )
  SHADER_TO_HEADER("ShadowMappingFragmentText"
		   "Shaders/ShadowMapping.frag"
		   "${vtkINRIA3D_BINARY_DIR}/Shaders/ShadowMappingFragmentText.h"
		  )
  SHADER_TO_HEADER("SpotlightFunctionsText"
		   "Shaders/Spotlight.func"
		   "${vtkINRIA3D_BINARY_DIR}/Shaders/SpotlightFunctionsText.h"
		  )
  SHADER_TO_HEADER("AnisotropicLightingFunctionsText"
		   "Shaders/AnisotropicLighting.func"
		   "${vtkINRIA3D_BINARY_DIR}/Shaders/AnisotropicLightingFunctionsText.h"
		  )
  SHADER_TO_HEADER("AnisotropicLightingShadowMappingFragmentText"
		   "Shaders/AnisoLiShadowMap.frag"
		   "${vtkINRIA3D_BINARY_DIR}/Shaders/AnisotropicLightingShadowMappingFragmentText.h"
		  )
  SHADER_TO_HEADER("AnisotropicLightingShadowMappingVertexText"
		   "Shaders/AnisoLiShadowMap.vert"
		   "${vtkINRIA3D_BINARY_DIR}/Shaders/AnisotropicLightingShadowMappingVertexText.h"
		  )
  SHADER_TO_HEADER("BuildShadowMapVertexText"
		   "Shaders/BuildShadowMap.vert"
		   "${vtkINRIA3D_BINARY_DIR}/Shaders/BuildShadowMapVertexText.h"
		  )
  SHADER_TO_HEADER("BuildShadowMapFragmentText"
		   "Shaders/BuildShadowMap.frag"
		   "${vtkINRIA3D_BINARY_DIR}/Shaders/BuildShadowMapFragmentText.h"
		  )
  SHADER_TO_HEADER("BuildShadowMapLinesVertexText"
		   "Shaders/BuildShadowMapLines.vert"
		   "${vtkINRIA3D_BINARY_DIR}/Shaders/BuildShadowMapLinesVertexText.h"
		  )
  SHADER_TO_HEADER("BuildShadowMapLinesFragmentText"
		   "Shaders/BuildShadowMapLines.frag"
		   "${vtkINRIA3D_BINARY_DIR}/Shaders/BuildShadowMapLinesFragmentText.h"
		  )


  ADD_LIBRARY(vtkHWShading ${vtkINRIA3D_LIBS_STYLE}
  ${HWShading_SRCS} 
  )

  TARGET_LINK_LIBRARIES(vtkHWShading 
  vtkRendering 
  vtkHelpers 
  )

  IF( BUILD_TESTING )
    ADD_SUBDIRECTORY( Testing )
  ENDIF( BUILD_TESTING )

  if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
    install(TARGETS vtkHWShading
    LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
    ARCHIVE DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
    )
  endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

  if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
    install_files(${vtkINRIA3D_INSTALL_INCLUDE_DIR} .h ${HWShading_SRCS})
  endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)


