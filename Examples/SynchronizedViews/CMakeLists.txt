PROJECT(SynchronizedViews)

FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

ADD_EXECUTABLE(SynchronizedViews
  SynchronizedViews.cxx
)  
TARGET_LINK_LIBRARIES(SynchronizedViews
  vtkRenderingAddOn
  vtkRendering
  vtkIO
  vtkDataManagement
)     
