project(RegisterDataSets)

include_directories(
  ../../vtkImageView
  ../../vtkDataManagement
  )

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

if (APPLE)
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    RegisterDataSets.cxx
    )  
else()
  add_executable(${PROJECT_NAME}
    RegisterDataSets.cxx
    )  
endif()

target_link_libraries(${PROJECT_NAME}
  vtkImageView
  vtkDataManagement
  )
