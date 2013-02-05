
OPTION(CardioViz3D_GENERICTOOLBOX "Build the generic toolbox for CardioViz3D" ON)

SET(GenericToolBox_SRCS
  vtkKWGenericToolBox.cxx
)


# Wrapping TCL

INCLUDE(
  "${KWWidgets_CMAKE_DIR}/KWWidgetsWrappingMacros.cmake"
)
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR})

KWWidgets_WRAP_TCL(GenericToolBox GenericToolBoxTCL_SRCS "${GenericToolBox_SRCS}" "")



ADD_LIBRARY(GenericToolBox
  ${GenericToolBoxTCL_SRCS}
  ${GenericToolBox_SRCS}
)


TARGET_LINK_LIBRARIES(GenericToolBox
  KWAddOn
  ${KWWidgets_LIBRARIES}
  vtkDataManagement
  vtkDataManagementTCL
  ${TCL_LIBRARY}
)

# Install the target. If you are using this CMakeLists.txt file
# to create your own application based on KWWidgets, you can most likely 
# omit this section or change the installation directory, unless you want
# to install your application right where KWWidgets is already installed.

INSTALL_FILES(/include "(\\.h|\\.txx)$")
INSTALL_TARGETS(/lib GenericToolBox)

