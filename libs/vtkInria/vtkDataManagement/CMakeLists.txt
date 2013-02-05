project(vtkDataManagement)

  SET(vtkDataManagement_SRCS
  vtkDataManagerCallback.cxx
  vtkMetaDataSet.cxx
  vtkMetaImageData.cxx
  vtkMetaSurfaceMesh.cxx
  vtkMetaVolumeMesh.cxx
  vtkDataManager.cxx
  vtkMetaDataSetSequence.cxx
  vtkDiffXMLWriter.cxx
  vtkFiberDataSet.cxx
  vtkXMLFiberDataSetReader.cxx
  vtkXMLFiberDataSetWriter.cxx
  )

  IF (VTK_MAJOR_VERSION GREATER 4)
    IF (VTK_MINOR_VERSION GREATER 0)
    SET(vtkDataManagement_SRCS
    ${vtkDataManagement_SRCS}
    vtkDataManagerWriter.cxx
    vtkDataManagerReader.cxx
    )
  ENDIF (VTK_MINOR_VERSION GREATER 0)
  ENDIF (VTK_MAJOR_VERSION GREATER 4)

if(vtkINRIA3D_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(vtkDataManagement_SOURCES_PCH 
    "vtkInria3dPch/vtkInria3dPch.h" "../vtkInria3dPch/vtkInria3dPch.cpp" 
    ${vtkDataManagement_SRCS})
endif(vtkINRIA3D_USE_PRECOMPILED_HEADERS)

  # C++ library for the InterfaceManagers
  ADD_LIBRARY(vtkDataManagement ${vtkINRIA3D_LIBS_STYLE}
  ${vtkDataManagement_SOURCES_PCH}
  ${vtkDataManagement_SRCS}
  )
  
  SET(vtkDataManagement_LIBS
    vtkCommon
    vtkRendering
    vtkWidgets
    vtkVolumeRendering
    vtkHybrid
    vtkImaging    
    vtkGraphics
    vtkFiltering
    vtkImaging
    vtkIO
    vtkRendering
  )
    
  IF(ITK_FOUND)
    
    SET(vtkDataManagement_LIBS
      itkIOAddOn
      ${vtkDataManagement_LIBS}
    )

    IF(ITK_USE_REVIEW)
      SET(vtkDataManagement_LIBS
        ITKIOReview
	${vtkDataManagement_LIBS}	
      )
    ENDIF(ITK_USE_REVIEW)
    
  ENDIF(ITK_FOUND)

  TARGET_LINK_LIBRARIES(vtkDataManagement
  ${vtkDataManagement_LIBS}
  )

  IF (VTK_WRAP_TCL)

    SET_SOURCE_FILES_PROPERTIES(
     vtkDataManagerCallback.cxx
     vtkFiberDataSet.cxx
     WRAP_EXCLUDE
    )
    
    VTK_WRAP_TCL3(
      vtkDataManagementLib
      vtkDataManagementTCL_SRCS
      "${vtkDataManagement_SRCS}"
      ""
      "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}"
    )

    ADD_LIBRARY(vtkDataManagementTCL ${vtkINRIA3D_LIBS_STYLE}
      ${vtkDataManagementTCL_SRCS}
    )
    TARGET_LINK_LIBRARIES(vtkDataManagementTCL
        vtkDataManagement
        vtkCommonTCL
        vtkRenderingTCL
        vtkWidgetsTCL
        vtkVolumeRenderingTCL
        vtkHybridTCL
        vtkImagingTCL
    )
  ENDIF(VTK_WRAP_TCL)

  IF( BUILD_TESTING )
    ADD_SUBDIRECTORY(Testing)
  ENDIF( BUILD_TESTING )


