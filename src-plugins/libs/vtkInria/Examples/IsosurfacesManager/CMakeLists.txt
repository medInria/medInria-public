PROJECT(IsosurfacesManager)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkVisuManagement
  )

ADD_EXECUTABLE(${PROJECT_NAME}
  IsosurfacesManager.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )


