PROJECT(wxImageViewsWithSliders)

FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})



IF( wxWidgets_FOUND )

  # special case of win32. It is for cmake to now which win32 specific
  # libraries to include.

  IF (APPLE)
     SET(GUI_EXECUTABLE MACOSX_BUNDLE)
  ENDIF(APPLE)

  # I create a library...
  ADD_LIBRARY(ImageViewsWithSlidersLib ${wxWidgets_LIBS_STYLE}
  wxImageViewsWithSlidersMainFrame.cxx
  )
  TARGET_LINK_LIBRARIES(ImageViewsWithSlidersLib
  ${wxWidgets_LIBRARIES}
  wxVtk
  wxAddOn
  ${ITK_LIBRARIES}
  vtkRenderingAddOn
  vtkDataManagement
  vtkRendering
  )
  
  SET(wxImageViewsWithSliders_SRCS
  wxImageViewsWithSlidersApp.cxx
  )

  IF( WIN32 )
    SET(GUI_EXECUTABLE WIN32)
  ENDIF( WIN32 )


  #... and an executable using the library
  ADD_EXECUTABLE(wxImageViewsWithSliders ${GUI_EXECUTABLE}
  ${wxImageViewsWithSliders_SRCS}
  )
  TARGET_LINK_LIBRARIES(wxImageViewsWithSliders
  ImageViewsWithSlidersLib
  ${wxWidgets_LIBRARIES}
  wxVtk
  wxAddOn
  ${ITK_LIBRARIES}
  vtkRenderingAddOn
  vtkRendering
  )

  INSTALL_TARGETS(/lib ImageViewsWithSlidersLib)
  INSTALL_TARGETS(/bin wxImageViewsWithSliders)

ENDIF( wxWidgets_FOUND )
