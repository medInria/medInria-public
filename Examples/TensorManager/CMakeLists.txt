PROJECT(TensorManager)

FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})

ADD_EXECUTABLE(TensorManager
TensorManager.cxx
)

TARGET_LINK_LIBRARIES(TensorManager
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
  