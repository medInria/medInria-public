  SET(vtkDataManagement_SRCS
  vtkDataManagerCallback.cxx
  vtkMetaDataSet.cxx
  vtkMetaImageData.cxx
  vtkMetaSurfaceMesh.cxx
  vtkMetaVolumeMesh.cxx
  vtkDataManager.cxx
  vtkMetaDataSetSequence.cxx
  vtkDiffXMLWriter.cxx
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


  # C++ library for the InterfaceManagers
  ADD_LIBRARY(vtkDataManagement ${vtkINRIA3D_LIBS_STYLE}
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
   
    if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
      install(TARGETS vtkDataManagementTCL
      LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
      RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
      ARCHIVE DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
      )
    endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

  ENDIF(VTK_WRAP_TCL)



  IF( BUILD_TESTING )
    ADD_SUBDIRECTORY(Testing)
  ENDIF( BUILD_TESTING )
  
  if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
    install(TARGETS vtkDataManagement
    LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}      
    ARCHIVE DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
    )
  endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

  if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
    install(DIRECTORY ./ 
        DESTINATION ${vtkINRIA3D_INSTALL_INCLUDE_DIR}
        FILES_MATCHING PATTERN "*.h")
  endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)

