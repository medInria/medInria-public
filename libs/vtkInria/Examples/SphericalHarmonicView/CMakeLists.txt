PROJECT(SHVisu)

include_directories(
../../vtkVisuManagement
../../vtkRenderingAddOn)

IF(APPLE)
SET(GUI_EXECUTABLE MACOSX_BUNDLE)
ENDIF(APPLE)

ADD_EXECUTABLE(SHVisu ${GUI_EXECUTABLE}
SHManager.cxx
)

TARGET_LINK_LIBRARIES(SHVisu
vtkVisuManagement
vtkRenderingAddOn
)

