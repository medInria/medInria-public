PROJECT(SynchronizedViews)

include_directories(
  ../../vtkImageView
  ../../vtkDataManagement
  )

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

if (APPLE)
  ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE
    SynchronizedViews.cxx
    )  
else(APPLE)
  ADD_EXECUTABLE(${PROJECT_NAME}
    SynchronizedViews.cxx
    )  
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  vtkImageView
  vtkDataManagement
  )
