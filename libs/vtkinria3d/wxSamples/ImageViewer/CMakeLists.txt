PROJECT(wxImageViewer)

FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})



IF( ITK_FOUND AND wxWidgets_FOUND AND NOT ITK_USE_SYSTEM_GDCM)

   ADD_LIBRARY(ImageViewer ${wxWidgets_LIBS_STYLE}
   wxImageViewer.cxx
   wxImageViewerInteractor.cxx
   wxImageThumbnail.cxx
   )
   TARGET_LINK_LIBRARIES(ImageViewer   
   ${wxWidgets_LIBRARIES}
   vtkVisuManagement
   vtkDataManagement
   wxVtk
   wxAddOn
   ${ITK_LIBRARIES}
   )

   SET(wxImageViewer_SRCS
   wxImageViewerApp.cxx
   )

   IF (WIN32)
      SET(GUI_EXECUTABLE WIN32)
      SET(wxImageViewer_SRCS
      wxImageViewer.rc
      ${wxImageViewer_SRCS}
      )
   ENDIF(WIN32)
   
   IF (APPLE)
     SET(GUI_EXECUTABLE MACOSX_BUNDLE)
	SET(MACOSX_BUNDLE_ICON_FILE "imageviewer.icns")
	SET(wxImageViewer_SRCS
	imageviewer.icns
	${wxImageViewer_SRCS}
	)
	SET_SOURCE_FILES_PROPERTIES(
        imageviewer.icns
        PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
        )
   ENDIF(APPLE)
  
   ADD_EXECUTABLE(wxImageViewer ${GUI_EXECUTABLE}
   ${wxImageViewer_SRCS}
   )


   SET(ImageViewer_LIBRARIES
     ImageViewer
     ${wxWidgets_LIBRARIES}
     vtkVisuManagement
     wxVtk
     wxAddOn
     ${ITK_LIBRARIES}    
   )

   TARGET_LINK_LIBRARIES(wxImageViewer
     ${ImageViewer_LIBRARIES}
   )

   ADD_EXECUTABLE(DICOMImporter ${GUI_EXECUTABLE}
   wxDICOMImporterApp.cxx
   )

   TARGET_LINK_LIBRARIES(DICOMImporter
   ${wxWidgets_LIBRARIES}
   wxVtk
   wxAddOn
   ${ITK_LIBRARIES}
   )


   INSTALL_FILES(/include "(\\.h|\\.txx)$")
   INSTALL_TARGETS(/bin wxImageViewer)
   INSTALL_TARGETS(/bin DICOMImporter)
   INSTALL_TARGETS(/lib ImageViewer)

ENDIF( ITK_FOUND AND wxWidgets_FOUND AND NOT ITK_USE_SYSTEM_GDCM)
