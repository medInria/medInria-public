project(IsosurfacesManager)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkVisuManagement
  )

add_executable(${PROJECT_NAME}
  IsosurfacesManager.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )


