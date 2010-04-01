PROJECT(wxHelloWorld)

FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})


IF( wxWidgets_FOUND )

  # special case of win32. It is for cmake to now which win32 specific
  # libraries to include.

  IF (APPLE)
     SET(GUI_EXECUTABLE MACOSX_BUNDLE)
  ENDIF(APPLE)

  # I create a library...
  ADD_LIBRARY(HelloWorldLib ${wxWidgets_LIBS_STYLE}
  wxHelloWorldMainFrame.cxx
  wxHelloWorldMainFrameInteractor.cxx
  )
  TARGET_LINK_LIBRARIES(HelloWorldLib
  ${wxWidgets_LIBRARIES}
  wxVtk
  wxAddOn
  ${ITK_LIBRARIES}
  vtkDataManagement
  vtkRendering
  )
  
  SET(wxHelloWorld_SRCS
  wxHelloWorldApp.cxx
  )

  IF( WIN32 )
    SET(GUI_EXECUTABLE WIN32)
    SET(wxHelloWorld_SRCS
    ${wxHelloWorld_SRCS}
    wxHelloWorld.rc
    )
  ENDIF( WIN32 )


  #... and an executable using the library
  ADD_EXECUTABLE(wxHelloWorld ${GUI_EXECUTABLE}
  ${wxHelloWorld_SRCS}
  )
  TARGET_LINK_LIBRARIES(wxHelloWorld
  HelloWorldLib
  ${wxWidgets_LIBRARIES}
  wxVtk
  wxAddOn
  ${ITK_LIBRARIES}
  vtkRendering
  )

  INSTALL_TARGETS(/lib HelloWorldLib)
  INSTALL_TARGETS(/bin wxHelloWorld)

ENDIF( wxWidgets_FOUND )
