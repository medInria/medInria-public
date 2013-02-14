PROJECT(SynchronizedViews)

include_directories(
../../vtkImageView
../../vtkDataManagement)

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
