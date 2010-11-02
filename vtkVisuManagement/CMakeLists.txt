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
 vtkLandmarkManager.cxx
 vtkLandmarkManagerCallback.cxx
)

SET(vtkVisuManagement_LIBS
 vtkCommon
 vtkFiltering 
 vtkRendering
 vtkWidgets
 vtkRenderingAddOn
)



IF( vtkINRIA3D_USE_HWSHADING )
SET(vtkVisuManagement_LIBS
${vtkVisuManagement_LIBS}
vtkHWShading
)
ENDIF (vtkINRIA3D_USE_HWSHADING )



# C++ library for the InterfaceManagers
ADD_LIBRARY(vtkVisuManagement ${vtkINRIA3D_LIBS_STYLE}
 ${vtkVisuManagement_SRCS}
)
  
TARGET_LINK_LIBRARIES(vtkVisuManagement  
 ${vtkVisuManagement_LIBS}
)

#  IF (VTK_WRAP_TCL)
#    
#    VTK_WRAP_TCL3( vtkVisuManagementLib
#      vtkVisuManagementTCL_SRCS
#      "vtkVectorVisuManager.cxx"
#      ""
#      "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}"
#    )
#
#    ADD_LIBRARY(vtkVisuManagementTCL ${vtkINRIA3D_LIBS_STYLE}
#    ${vtkVisuManagementTCL_SRCS}
#    )
#    TARGET_LINK_LIBRARIES(vtkVisuManagementTCL
#    vtkVisuManagement
#    vtkCommonTCL
#    vtkRenderingTCL
#    vtkWidgetsTCL
#    vtkVolumeRenderingTCL
#    vtkHybridTCL
#    vtkImagingTCL
#    )
# 
#    INSTALL_TARGETS(/lib vtkVisuManagementTCL)
#
#  ENDIF(VTK_WRAP_TCL)

if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
  install(TARGETS vtkVisuManagement
  LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
  ARCHIVE DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
  )
endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
  install(DIRECTORY 
      DESTINATION ${vtkINRIA3D_INSTALL_INCLUDE_DIR}
        FILES_MATCHING PATTERN "*.h")
endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
