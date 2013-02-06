project(vtkDataManagement)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

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

if(vtkInria_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(vtkDataManagement_SOURCES_PCH 
    "vtkInria3dPch/vtkInria3dPch.h" "../vtkInria3dPch/vtkInria3dPch.cpp" 
    ${vtkDataManagement_SRCS})
endif(vtkInria_USE_PRECOMPILED_HEADERS)

  # C++ library for the InterfaceManagers
  ADD_LIBRARY(vtkDataManagement ${vtkInria_LIBS_STYLE}
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
    
  SET(vtkDataManagement_LIBS
      itkIOAddOn
      ${vtkDataManagement_LIBS}
    )

  SET(vtkDataManagement_LIBS
        ITKIOReview
	${vtkDataManagement_LIBS}	
   )
    

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

    ADD_LIBRARY(vtkDataManagementTCL ${vtkInria_LIBS_STYLE}
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


