PROJECT(SynchronizedViews)

include_directories(
  ../../vtkImageView
  ../../vtkDataManagement
  )

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

if (APPLE)
  add_execitable(${PROJECT_NAME} MACOSX_BUNDLE
    SynchronizedViews.cxx
    )  
else()
  add_execitable(${PROJECT_NAME}
    SynchronizedViews.cxx
    )  
endif()

target_link_libraries(${PROJECT_NAME}
  vtkImageView
  vtkDataManagement
  )
