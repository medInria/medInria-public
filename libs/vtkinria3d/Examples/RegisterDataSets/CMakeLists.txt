PROJECT(RegisterDataSets)

FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

if (APPLE)
  ADD_EXECUTABLE(RegisterDataSets MACOSX_BUNDLE
    RegisterDataSets.cxx
  )  
else(APPLE)
  ADD_EXECUTABLE(RegisterDataSets
    RegisterDataSets.cxx
  )  
endif()

TARGET_LINK_LIBRARIES(RegisterDataSets
  vtkImageView
  vtkDataManagement
)