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

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )

ADD_EXECUTABLE(SyncTensors
  SyncTensors.cxx
  )	    
    
TARGET_LINK_LIBRARIES(SyncTensors
  vtkRendering
  vtkIO
  vtkImageView
  vtkDataManagement
  vtkVisuManagement
  ${ITK_LIBRARIES}
  )
  
