PROJECT(TensorManager)

include_directories(
  ../../vtkImageView
  ../../vtkRenderingAddOn
  ../../vtkDataManagement
  ../../vtkVisuManagement
  )

ADD_EXECUTABLE(${PROJECT_NAME}
  TensorManager.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )

ADD_EXECUTABLE(SyncTensors
  SyncTensors.cxx
  )	    
    
target_link_libraries(SyncTensors
  vtkRendering
  vtkIO
  vtkImageView
  vtkDataManagement
  vtkVisuManagement
  ${ITK_LIBRARIES}
  )
  
