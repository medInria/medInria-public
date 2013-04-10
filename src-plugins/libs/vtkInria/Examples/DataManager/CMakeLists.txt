PROJECT(DataManager)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkDataManagement
  ../../vtkVisuManagement
  )

IF(APPLE)
  set(GUI_EXECUTABLE MACOSX_BUNDLE)
endif()

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


