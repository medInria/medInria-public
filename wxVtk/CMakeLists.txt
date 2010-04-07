IF(wxWidgets_FOUND)
  
  ####################################################
  # The unix version needs the gtk includes to compile
  # the wx-VTK interface.
  IF( UNIX AND NOT APPLE)
    
    INCLUDE_DIRECTORIES(
      ${GTK_INCLUDE_DIRS}
      )
    ADD_DEFINITIONS(${GTK_CFLAGS})
    
  ENDIF(UNIX AND NOT APPLE)
  
  SET(wxVtk_SRCS
    wxVTKRenderWindowInteractor.cxx
    wxDialogVtkViewImage2D.cxx
    wxVtkImage3DImagePlaneCallback.cxx
    wxSnapshotTaker.cxx
    wxVtkLandmarkWindow.cxx
    wxVtkPageView.cxx
    wxVtkViewImage2DWithSliders.cxx
    )
  
  
  SET(wxVtk_LIBS
    vtkCommon
    vtkRendering
    vtkDataManagement
    vtkVisuManagement
    vtkRenderingAddOn
    )
  
  
  IF(ITK_FOUND)
    
    SET(wxVtk_SRCS
      wxVtkImageFlip.cxx
      wxItkProgressDialogCallback.cxx
      wxItkProgressDialog.cxx
      ${wxVtk_SRCS}
      )
    IF (NOT ITK_USE_SYSTEM_GDCM)
      SET(wxVtk_SRCS
	${wxVtk_SRCS}
	wxVtkDICOMExporter.cxx
	wxVtkDICOMImporter.cxx
	wxVtkDICOMImporterPage2.cxx
	wxDicomInfoDialog.cxx
	)
    ENDIF(NOT ITK_USE_SYSTEM_GDCM)
    
    SET(wxVtk_LIBS
      itkAddOn
      ${ITK_LIBRARIES}
      ${wxVtk_LIBS}
      )
    
  ENDIF(ITK_FOUND)
  
  ADD_LIBRARY(wxVtk ${wxWidgets_LIBS_STYLE}
    ${wxVtk_SRCS}
    )
  
  TARGET_LINK_LIBRARIES(wxVtk
    ${wxVtk_LIBS}
    ${wxWidgets_LIBRARIES}
    wxAddOn
    )
  
  if (BUILD_TESTING)
    add_subdirectory(Testing)
  endif (BUILD_TESTING)
  
  if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
    install_targets(
      ${vtkINRIA3D_INSTALL_LIB_DIR}
      RUNTIME_DIRECTORY ${vtkINRIA3D_INSTALL_BIN_DIR}
      wxVtk
      )
  endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
  
  if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
    install_files(${vtkINRIA3D_INSTALL_INCLUDE_DIR} .h ${wxVtk_SRCS})
  endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
  
ENDIF(wxWidgets_FOUND)
