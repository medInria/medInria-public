project(vtkVisuManagement)

SET(vtkVisuManagement_SRCS
 vtkLimitFibersToVOI.cxx
 vtkLimitFibersToMesh.cxx
 vtkLimitFibersToROI.cxx
 vtkSqueezeFibers.cxx
 vtkFibersManager.cxx
 vtkImageToIsosurface.cxx
 vtkIsosurfaceManager.cxx
 vtkPolyDataManager.cxx
 vtkImageMerge.cxx  
 vtkFlipTensorImageFilter.cxx
 vtkCompareImageManager.cxx
 vtkLookupTableManager.cxx
 vtkTransferFunctionPresets.cxx
 vtkTensorVisuManager.cxx
 vtkTensorManager.cxx
 vtkVectorVisuManager.cxx
 vtkLimitVectorsToVOI.cxx
 vtkDatasetToImageGenerator.cxx
 vtkImageClamp.cxx
 vtkImageFuse.cxx
 vtkImageClippingPlane.cxx
 vtkDisplacementFieldToGridFilter.cxx
 vtkMeshCropManager.cxx
# vtkLandmarkManager.cxx
# vtkLandmarkManagerCallback.cxx
 vtkFiberDataSetManager.cxx
)

SET(vtkLandmarkManagement_SRCS
 vtkLandmarkManager.cxx
 vtkLandmarkManagerCallback.cxx
)

SET(vtkVisuManagement_LIBS
 vtkCommon
 vtkFiltering 
 vtkRendering
 vtkWidgets
 vtkDataManagement
)

SET(vtkLandmarkManagement_LIBS
 vtkCommon
 vtkFiltering
 vtkRendering
 vtkWidgets
 vtkDataManagement
 vtkImageView
)

IF( vtkINRIA3D_USE_HWSHADING )
  SET(vtkVisuManagement_LIBS
    ${vtkVisuManagement_LIBS}
    vtkHWShading
    )
  SET(vtkLandmarkManagement_LIBS
    ${vtkLandmarkManagement_LIBS}
    vtkHWShading
    )
ENDIF(vtkINRIA3D_USE_HWSHADING )

IF( vtkINRIA3D_USE_SPHERICALHARMONICS )
    SET(vtkVisuManagement_LIBS
        ${vtkVisuManagement_LIBS}
        SphericalHarmonicsVisu
       )
    SET(vtkVisuManagement_SRCS
        ${vtkVisuManagement_SRCS}
        vtkSphericalHarmonicVisuManager.cxx
        vtkSphericalHarmonicManager.cxx
        )
ENDIF(vtkINRIA3D_USE_SPHERICALHARMONICS )

if(vtkINRIA3D_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(vtkVisuManagement_SOURCES_PCH 
    "vtkInria3dPch/vtkInria3dPch.h" "../vtkInria3dPch/vtkInria3dPch.cpp" 
      ${vtkVisuManagement_SRCS}
    )
endif(vtkINRIA3D_USE_PRECOMPILED_HEADERS)

# C++ library for the InterfaceManagers
ADD_LIBRARY(vtkVisuManagement ${vtkINRIA3D_LIBS_STYLE}
  ${vtkVisuManagement_SOURCES_PCH}
  ${vtkVisuManagement_SRCS}
)
  
# C++ library for the InterfaceManagers
ADD_LIBRARY(vtkLandmarkManagement ${vtkINRIA3D_LIBS_STYLE}
  ${vtkLandmarkManagement_SRCS}
)
  
TARGET_LINK_LIBRARIES(vtkVisuManagement  
  ${vtkVisuManagement_LIBS}
)

  
TARGET_LINK_LIBRARIES(vtkLandmarkManagement  
  ${vtkLandmarkManagement_LIBS}
)

IF(VTK_WRAP_TCL)
  SET_SOURCE_FILES_PROPERTIES(
     vtkLandmarkManagerCallback.cxx
     WRAP_EXCLUDE
    )
    
  VTK_WRAP_TCL3( vtkVisuManagementLib
      vtkVisuManagementTCL_SRCS
      "${vtkVisuManagement_SRCS}"
      ""
      "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}"
    )
  ADD_LIBRARY(vtkVisuManagementTCL ${vtkINRIA3D_LIBS_STYLE}
    ${vtkVisuManagementTCL_SRCS}
    )
  TARGET_LINK_LIBRARIES(vtkVisuManagementTCL
    vtkVisuManagement
    vtkCommonTCL
    vtkRenderingTCL
    vtkWidgetsTCL
    vtkVolumeRenderingTCL
    vtkHybridTCL
    vtkImagingTCL
    )
 
  INSTALL_TARGETS(/lib vtkVisuManagementTCL)

  VTK_WRAP_TCL3( vtkLandmarkManagementLib
      vtkLandmarkManagementTCL_SRCS
      "${vtkLandmarkManagement_SRCS}"
      ""
      "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}"
    )
  ADD_LIBRARY(vtkLandmarkManagementTCL ${vtkINRIA3D_LIBS_STYLE}
    ${vtkLandmarkManagementTCL_SRCS}
    )
  TARGET_LINK_LIBRARIES(vtkLandmarkManagementTCL
    vtkLandmarkManagement
    vtkCommonTCL
    vtkRenderingTCL
    vtkWidgetsTCL
    vtkVolumeRenderingTCL
    vtkHybridTCL
    vtkImagingTCL
    )
 
  INSTALL_TARGETS(/lib vtkLandmarkManagementTCL)
ENDIF(VTK_WRAP_TCL)

if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
  install(TARGETS vtkVisuManagement
  LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
  ARCHIVE DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
  )
endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
  install(DIRECTORY ./
      DESTINATION ${vtkINRIA3D_INSTALL_INCLUDE_DIR}
        FILES_MATCHING PATTERN "*.h")
endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
