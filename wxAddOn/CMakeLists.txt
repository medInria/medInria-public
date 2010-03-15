IF(wxWidgets_FOUND)

  ADD_DEFINITIONS(-DCHECKBOXVIEW=1)

  ADD_LIBRARY(wxAddOn ${wxWidgets_LIBS_STYLE}
		wxAboutDialog.cxx
		#TreeMultiItemBase.cpp 
		#TreeMultiItemNode.cpp 
		#TreeMultiItemWindow.cpp
		#TreeMultiItemRoot.cpp 
		#wxTreeMultiCtrl.cpp 
		captionbar.cpp
		foldpanelbar.cpp
		foldpanelitem.cpp
		progdlg2.cpp
		styled_notebook.cpp
		wxFlatNotebook.cpp
		sliderff.cpp
		bmpbuttnff.cpp
		)

	TARGET_LINK_LIBRARIES(wxAddOn
		${wxWidgets_LIBRARIES}
		)  
  

  IF( BUILD_TESTING )
    ADD_SUBDIRECTORY(Testing)
  ENDIF( BUILD_TESTING )

  if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
    install_targets(
    ${vtkINRIA3D_INSTALL_LIB_DIR}
    RUNTIME_DIRECTORY ${vtkINRIA3D_INSTALL_BIN_DIR}
    wxAddOn
    )
  endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

  IF(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
    FILE(GLOB __files1 "${CMAKE_CURRENT_SOURCE_DIR}/wx/*.h")
    FILE(GLOB __files2 "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    INSTALL(FILES ${__files1}
      DESTINATION ${CMAKE_INSTALL_PREFIX}/${vtkINRIA3D_INSTALL_INCLUDE_DIR}/wx)
    INSTALL(FILES ${__files2}
      DESTINATION ${CMAKE_INSTALL_PREFIX}/${vtkINRIA3D_INSTALL_INCLUDE_DIR})
  ENDIF(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)

ENDIF(wxWidgets_FOUND)

