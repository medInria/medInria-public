PROJECT(FibersManager)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkVisuManagement
  )

IF(APPLE)
  SET(GUI_EXECUTABLE MACOSX_BUNDLE)
endif()

ADD_EXECUTABLE(${PROJECT_NAME} 
  ${GUI_EXECUTABLE}
  FibersManager.cxx
  )

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )


