# KWWidgets 



IF( KWWidgets_FOUND AND ITK_FOUND)

INCLUDE_DIRECTORIES(
  ${TCL_INCLUDE_PATH}
)
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR})

SET(KWAddOn_SRCS
  vtkKWMainCallback.cxx
#  vtkKWImage3DImagePlaneCallback.cxx
#  vtkKWViewImage2DDialog.cxx
#  vtkKWMetaInfoWidget.cxx
  vtkKWTimeAnimationWidget.cxx
  vtkKWCameraAnimationWidget.cxx
  vtkKWSequenceImporter.cxx
  vtkKWSequencer.cxx
#  vtkKWMetaDataSetControlWidget.cxx
  vtkKWToolBox.cxx
  vtkKWToolboxManager.cxx
  vtkKWHelloWorldToolBox.cxx
  vtkKWPageView.cxx
  vtkKWPreviewPage.cxx
  vtkKWMainWindow.cxx
  vtkKWMainWindowInteractor.cxx
  vtkKWDICOMImporter2.cxx
  vtkKWVisualizationToolBox.cxx
  vtkKWDataManagerWidget.cxx
  vtkKWLandmarkManagerWidget.cxx
  vtkKWDicomInfoWidget.cxx
  vtkKWInfoToolBox.cxx
  vtkKWDataSelectorWidget.cxx
  vtkKWDICOMExporter.cxx
  vtkKWSequenceEditorWidget.cxx
  vtkKWTimeTableWidget.cxx
  vtkKWSnapshotHandler.cxx
)
SET_SOURCE_FILES_PROPERTIES(
 vtkKWMainCallback.cxx
# vtkKWImage3DImagePlaneCallback.cxx
 WRAP_EXCLUDE
)

# Wrapping TCL

INCLUDE(
  "${KWWidgets_CMAKE_DIR}/KWWidgetsWrappingMacros.cmake"
)
KWWidgets_WRAP_TCL(KWAddOn KWAddOnTCL_SRCS "${KWAddOn_SRCS}" "")



ADD_LIBRARY(KWAddOn ${vtkINRIA3D_LIBS_STYLE}
  ${KWAddOnTCL_SRCS}
  ${KWAddOn_SRCS}
)

SET(KWAddOn_LIBRARIES
  vtkCommon
  vtkDataManagement
  vtkDataManagementTCL
  ${vtkINRIA3D_LIB_RENDERING}
  ${vtkINRIA3D_LIB_RENDERING}TCL
  vtkVisuManagement
)

TARGET_LINK_LIBRARIES(KWAddOn
  ${KWAddOn_LIBRARIES}
  ${KWWidgets_LIBRARIES}
)




INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(
  ${KWWidgets_TEMPLATES_DIR}/vtkKWWidgetsPaths.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkKWWidgetsPaths.h)


# If needed, copy the Tcl/Tk support files required at run-time 
# to initialize Tcl/Tk. This is only triggered if VTK was built
# against a Tcl/Tk static library.

IF(NOT KWWidgets_SOURCE_DIR AND VTK_TCL_TK_COPY_SUPPORT_LIBRARY)
  IF(VTK_TCL_SUPPORT_LIBRARY_PATH AND VTK_TK_SUPPORT_LIBRARY_PATH)
    INCLUDE(${VTK_TCL_TK_MACROS_MODULE})
    VTK_COPY_TCL_TK_SUPPORT_FILES_TO_DIR(
      ${VTK_TCL_SUPPORT_LIBRARY_PATH} ${VTK_TK_SUPPORT_LIBRARY_PATH}
      "${PROJECT_BINARY_DIR}/lib")
  ENDIF(VTK_TCL_SUPPORT_LIBRARY_PATH AND VTK_TK_SUPPORT_LIBRARY_PATH)
ENDIF(NOT KWWidgets_SOURCE_DIR AND VTK_TCL_TK_COPY_SUPPORT_LIBRARY)

# Install the target. If you are using this CMakeLists.txt file
# to create your own application based on KWWidgets, you can most likely 
# omit this section or change the installation directory, unless you want
# to install your application right where KWWidgets is already installed.

INSTALL_FILES(/include "(\\.h|\\.txx)$")
INSTALL_FILES(/include/pixmap "pixmap/(\\.h|\\.txx)$")

INSTALL_TARGETS(/lib KWAddOn)




# ------------------------------------------------------------
# Creation of the binary executable and its launcher
# ------------------------------------------------------------

IF(WIN32 AND NOT BORLAND AND NOT CYGWIN)
  INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsResourceMacros.cmake")
  SET(RC_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/INRIAviz4D.rc")
  KWWidgets_CREATE_RC_FILE(
    RC_FILENAME "${RC_FILENAME}"
    RC_APPLICATION_NAME "INRIAviz4D"
    RC_COMPANY_NAME "Kitware, Inc.")
ENDIF(WIN32 AND NOT BORLAND AND NOT CYGWIN)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(
  ${KWWidgets_TEMPLATES_DIR}/vtkKWWidgetsPaths.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkKWWidgetsPaths.h)


ADD_EXECUTABLE(INRIAviz4D WIN32 
INRIAviz4D.cxx
#${RC_FILENAME}
)

SET(INRIAviz4D_LIBRARIES
  KWAddOn
)


IF(ITK_USE_REVIEW)
  SET(INRIAviz4D_LIBRARIES
    ITKIOReview
    ${INRIAviz4D_LIBRARIES}	
    )
ENDIF(ITK_USE_REVIEW)


TARGET_LINK_LIBRARIES(INRIAviz4D
  ${INRIAviz4D_LIBRARIES}
)

# If we are building this example as a standalone external project:
# - Generate a few small scripts (.bat, .sh, .csh) that can be sourced to set
# the various environments variables (PATH, TCLLIBPATH, LD_LIBRARY_PATH, etc.) 
# required by this executable and its known third-party dependencies (VTK, ITK,
# SOV, KWWidgets, etc.).
# - Generate a lightweight C launcher for this *specific* executable: It sets
# the above environment variables before launching the executable itself.

IF(NOT KWWidgets_SOURCE_DIR)
  INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsPathsMacros.cmake")
  KWWidgets_GENERATE_SETUP_PATHS_SCRIPTS("${CMAKE_CURRENT_BINARY_DIR}")
  SET(LAUNCHER_EXE_NAME "INRIAviz4DLauncher")
  KWWidgets_GENERATE_SETUP_PATHS_LAUNCHER(
    "${CMAKE_CURRENT_BINARY_DIR}" "${LAUNCHER_EXE_NAME}" "" "INRIAviz4D")
ENDIF(NOT KWWidgets_SOURCE_DIR)

# If needed, copy the Tcl/Tk support files required at run-time 
# to initialize Tcl/Tk. This is only triggered if VTK was built
# against a Tcl/Tk static library.

IF(NOT KWWidgets_SOURCE_DIR AND VTK_TCL_TK_COPY_SUPPORT_LIBRARY)
  IF(VTK_TCL_SUPPORT_LIBRARY_PATH AND VTK_TK_SUPPORT_LIBRARY_PATH)
    INCLUDE(${VTK_TCL_TK_MACROS_MODULE})
    VTK_COPY_TCL_TK_SUPPORT_FILES_TO_DIR(
      ${VTK_TCL_SUPPORT_LIBRARY_PATH} ${VTK_TK_SUPPORT_LIBRARY_PATH}
      "${PROJECT_BINARY_DIR}/lib")
  ENDIF(VTK_TCL_SUPPORT_LIBRARY_PATH AND VTK_TK_SUPPORT_LIBRARY_PATH)
ENDIF(NOT KWWidgets_SOURCE_DIR AND VTK_TCL_TK_COPY_SUPPORT_LIBRARY)

# Install the target. If you are using this CMakeLists.txt file
# to create your own application based on KWWidgets, you can most likely 
# omit this section or change the installation directory, unless you want
# to install your application right where KWWidgets is already installed.

INSTALL_TARGETS(/bin INRIAviz4D)
INSTALL_TARGETS(/bin INRIAviz4DLauncher)

SUBDIRS(
GenericToolBox
)

IF( BUILD_TESTING )
  ADD_SUBDIRECTORY(Testing)
ENDIF( BUILD_TESTING )

ENDIF( KWWidgets_FOUND AND ITK_FOUND)
