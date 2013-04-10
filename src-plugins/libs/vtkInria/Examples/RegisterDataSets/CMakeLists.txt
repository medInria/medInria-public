PROJECT(RegisterDataSets)

include_directories(
  ../../vtkImageView
  ../../vtkDataManagement
  )

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

if (APPLE)
  ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE
    RegisterDataSets.cxx
    )  
else()
  ADD_EXECUTABLE(${PROJECT_NAME}
    RegisterDataSets.cxx
    )  
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  vtkImageView
  vtkDataManagement
  )
