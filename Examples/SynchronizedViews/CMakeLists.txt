PROJECT(SynchronizedViews)

FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

if (APPLE)
  ADD_EXECUTABLE(SynchronizedViews MACOSX_BUNDLE
    SynchronizedViews.cxx
  )  
else(APPLE)
  ADD_EXECUTABLE(SynchronizedViews
    SynchronizedViews.cxx
  )  
endif()

TARGET_LINK_LIBRARIES(SynchronizedViews
  vtkRenderingAddOn
  vtkRendering
  vtkIO
  vtkDataManagement
)     

if (APPLE)
  add_executable(SynchronizedViews2
    SynchronizedViews2.cxx
  )
else(APPLE)
  add_executable(SynchronizedViews2 MACOSX_BUNDLE
    SynchronizedViews2.cxx
  )
endif()

target_link_libraries(SynchronizedViews2
 vtkImageView
)
