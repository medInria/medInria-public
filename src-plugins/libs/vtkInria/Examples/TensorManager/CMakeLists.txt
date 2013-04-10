PROJECT(TensorManager)

include_directories(
  ../../vtkImageView
  ../../vtkRenderingAddOn
  ../../vtkDataManagement
  ../../vtkVisuManagement
  )

add_execitable(${PROJECT_NAME}
  TensorManager.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )

add_execitable(SyncTensors
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
  
