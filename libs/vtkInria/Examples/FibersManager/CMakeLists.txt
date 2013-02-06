PROJECT(FibersManager)

FIND_PACKAGE(vtkInria REQUIRED)
INCLUDE(${vtkInria_USE_FILE})

IF(APPLE)
SET(GUI_EXECUTABLE MACOSX_BUNDLE)
ENDIF(APPLE)

ADD_EXECUTABLE(FibersManager ${GUI_EXECUTABLE}
FibersManager.cxx
)

TARGET_LINK_LIBRARIES(FibersManager
vtkRenderingAddOn
vtkVisuManagement
vtkRendering
vtkIO
)


