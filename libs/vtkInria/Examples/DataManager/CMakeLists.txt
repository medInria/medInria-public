PROJECT(DataManager)

FIND_PACKAGE(vtkInria REQUIRED)
INCLUDE(${vtkInria_USE_FILE})

IF(APPLE)
SET(GUI_EXECUTABLE MACOSX_BUNDLE)
ENDIF(APPLE)

ADD_EXECUTABLE(DataManager ${GUI_EXECUTABLE}
DataManager.cxx
)

TARGET_LINK_LIBRARIES(DataManager
vtkDataManagement
vtkRenderingAddOn
vtkVisuManagement
vtkRendering
vtkIO
)

ADD_EXECUTABLE(FiberDataSetReader
FiberDataSetReader.cxx
)

TARGET_LINK_LIBRARIES(FiberDataSetReader
vtkDataManagement
vtkRenderingAddOn
vtkVisuManagement
vtkRendering
vtkIO
)


