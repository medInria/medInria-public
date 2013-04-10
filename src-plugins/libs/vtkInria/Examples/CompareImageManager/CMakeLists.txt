PROJECT(CompareImageManager)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkVisuManagement
  )

ADD_EXECUTABLE(${PROJECT_NAME}
CompareImageManager.cxx
)

target_link_libraries(${PROJECT_NAME}
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )




