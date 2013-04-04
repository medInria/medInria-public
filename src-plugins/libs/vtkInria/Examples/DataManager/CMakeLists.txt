PROJECT(DataManager)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkDataManagement
  ../../vtkVisuManagement
  )

IF(APPLE)
  SET(GUI_EXECUTABLE MACOSX_BUNDLE)
ENDIF(APPLE)

ADD_EXECUTABLE(${PROJECT_NAME} ${GUI_EXECUTABLE}
  DataManager.cxx
  )

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
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


