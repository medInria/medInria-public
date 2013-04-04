PROJECT(CompareImageManager)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkVisuManagement
  )

ADD_EXECUTABLE(${PROJECT_NAME}
CompareImageManager.cxx
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )




