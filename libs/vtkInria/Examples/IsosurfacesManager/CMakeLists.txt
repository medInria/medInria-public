PROJECT(IsosurfacesManager)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkVisuManagement
  )

ADD_EXECUTABLE(${PROJECT_NAME}
  IsosurfacesManager.cxx
  )

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )


