project(vtkVisuManagement)

  IF (CMAKE_CXX_COMPILER MATCHES clang)
    add_definitions(-DCLANG)
  endif()

SET(${PROJECT_NAME}_SRCS
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
 vtkFiberDataSetManager.cxx
)

SET(vtkLandmarkManagement_SRCS
 vtkLandmark.cxx
 vtkLandmarkManager.cxx
 vtkLandmarkManagerCallback.cxx
)

SET(${PROJECT_NAME}_LIBS
 vtkCommon
 vtkFiltering 
 vtkRendering
 vtkWidgets
 vtkDataManagement
 vtkHWShading
)

SET(vtkLandmarkManagement_LIBS
 vtkCommon
 vtkFiltering
 vtkRendering
 vtkWidgets
 vtkDataManagement
 vtkImageView
 vtkHWShading
)

IF( VTKINRIA_USE_SPHERICALHARMONICS )
    SET(${PROJECT_NAME}_SRCS
        ${${PROJECT_NAME}_SRCS}
        vtkSphericalHarmonicVisuManager.cxx
        vtkSphericalHarmonicManager.cxx
        vtkTessellatedSphereSource.cxx
        vtkSphericalHarmonicSource.cxx
        vtkSphericalHarmonicGlyph.cxx
        )
    SET(${PROJECT_NAME}_LIBS
        ${${PROJECT_NAME}_LIBS}
        itkIOAddOn
        )
endif(VTKINRIA_USE_SPHERICALHARMONICS )

if (VTKINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH 
    "vtkInriaPch/vtkInriaPch.h" "../vtkInriaPch/vtkInriaPch.cpp" 
      ${${PROJECT_NAME}_SRCS}
    )
endif()

# C++ library for the InterfaceManagers
ADD_LIBRARY(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SRCS}
)
  
# C++ library for the InterfaceManagers
ADD_LIBRARY(vtkLandmarkManagement SHARED
  ${vtkLandmarkManagement_SRCS}
)
  
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  
  ${${PROJECT_NAME}_LIBS}
)

  
TARGET_LINK_LIBRARIES(vtkLandmarkManagement  
  ${vtkLandmarkManagement_LIBS}
)

INSTALL_TARGETS(/lib vtkLandmarkManagement)

IF(VTKINRIA_BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
endif(VTKINRIA_BUILD_TESTING)
