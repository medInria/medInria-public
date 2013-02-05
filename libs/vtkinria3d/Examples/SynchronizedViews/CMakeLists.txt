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
  vtkImageView
  vtkDataManagement
)