project(vtkVisuManagement)

  if (CMAKE_CXX_COMPILER MATCHES clang)
    add_definitions(-DCLANG)
  endif()

set(${PROJECT_NAME}_SRCS
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

set(vtkLandmarkManagement_SRCS
 vtkLandmark.cxx
 vtkLandmarkManager.cxx
 vtkLandmarkManagerCallback.cxx
)

set(${PROJECT_NAME}_LIBS
 vtkCommon
 vtkFiltering 
 vtkRendering
 vtkWidgets
 vtkDataManagement
 vtkHWShading
)

set(vtkLandmarkManagement_LIBS
 vtkCommon
 vtkFiltering
 vtkRendering
 vtkWidgets
 vtkDataManagement
 vtkImageView
 vtkHWShading
)

if ( VTKINRIA_USE_SPHERICALHARMONICS )
    set(${PROJECT_NAME}_SRCS
        ${${PROJECT_NAME}_SRCS}
        vtkSphericalHarmonicVisuManager.cxx
        vtkSphericalHarmonicManager.cxx
        vtkTessellatedSphereSource.cxx
        vtkSphericalHarmonicSource.cxx
        vtkSphericalHarmonicGlyph.cxx
        )
    set(${PROJECT_NAME}_LIBS
        ${${PROJECT_NAME}_LIBS}
        itkIOAddOn
        )
endif()

if (VTKINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH 
    "vtkInriaPch/vtkInriaPch.h" "../vtkInriaPch/vtkInriaPch.cpp" 
      ${${PROJECT_NAME}_SRCS}
    )
endif()

# C++ library for the InterfaceManagers
add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SRCS}
)
  
# C++ library for the InterfaceManagers
add_library(vtkLandmarkManagement SHARED
  ${vtkLandmarkManagement_SRCS}
)
  
target_link_libraries(${PROJECT_NAME}  
  ${${PROJECT_NAME}_LIBS}
)

  
target_link_libraries(vtkLandmarkManagement  
  ${vtkLandmarkManagement_LIBS}
)

INSTALL_TARGETS(/lib vtkLandmarkManagement)

if (VTKINRIA_BUILD_TESTING)
  add_subdirectory(Testing)
endif()
