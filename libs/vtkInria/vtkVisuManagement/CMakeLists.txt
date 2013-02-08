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
 vtkImageFromBoundsSource.cxx
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
 vtkLandmark.cxx
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

IF( vtkInria_USE_HWSHADING )
  SET(vtkVisuManagement_LIBS
    ${vtkVisuManagement_LIBS}
    vtkHWShading
    )
  SET(vtkLandmarkManagement_LIBS
    ${vtkLandmarkManagement_LIBS}
    vtkHWShading
    )
ENDIF(vtkInria_USE_HWSHADING )

IF( vtkInria_USE_SPHERICALHARMONICS )
    SET(vtkVisuManagement_SRCS
        ${vtkVisuManagement_SRCS}
        vtkSphericalHarmonicVisuManager.cxx
        vtkSphericalHarmonicManager.cxx
        vtkTessellatedSphereSource.cxx
        vtkSphericalHarmonicSource.cxx
        vtkSphericalHarmonicGlyph.cxx
        )
    SET(vtkVisuManagement_LIBS
        ${vtkVisuManagement_LIBS}
        itkIOAddOn
        )
ENDIF(vtkInria_USE_SPHERICALHARMONICS )

if(vtkInria_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(vtkVisuManagement_SOURCES_PCH 
    "vtkInria3dPch/vtkInria3dPch.h" "../vtkInria3dPch/vtkInria3dPch.cpp" 
      ${vtkVisuManagement_SRCS}
    )
endif(vtkInria_USE_PRECOMPILED_HEADERS)

# C++ library for the InterfaceManagers
ADD_LIBRARY(vtkVisuManagement SHARED
  ${vtkVisuManagement_SOURCES_PCH}
  ${vtkVisuManagement_SRCS}
)
  
# C++ library for the InterfaceManagers
ADD_LIBRARY(vtkLandmarkManagement SHARED
  ${vtkLandmarkManagement_SRCS}
)
  
TARGET_LINK_LIBRARIES(vtkVisuManagement  
  ${vtkVisuManagement_LIBS}
)

  
TARGET_LINK_LIBRARIES(vtkLandmarkManagement  
  ${vtkLandmarkManagement_LIBS}
)

INSTALL_TARGETS(/lib vtkLandmarkManagement)


IF(VTK_WRAP_TCL)
  SET_SOURCE_FILES_PROPERTIES(
     vtkLandmarkManagerCallback.cxx
     vtkLandmark.cxx
     vtkTessellatedSphereSource.cxx
     vtkSphericalHarmonicSource.cxx
     vtkSphericalHarmonicGlyph.cxx
     WRAP_EXCLUDE
    )
    
  VTK_WRAP_TCL3( vtkVisuManagementLib
      vtkVisuManagementTCL_SRCS
      "${vtkVisuManagement_SRCS}"
      ""
      "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}"
    )
  ADD_LIBRARY(vtkVisuManagementTCL SHARED
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
  ADD_LIBRARY(vtkLandmarkManagementTCL SHARED
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


IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
