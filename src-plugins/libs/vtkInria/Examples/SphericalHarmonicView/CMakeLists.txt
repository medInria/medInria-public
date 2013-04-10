PROJECT(SHVisu)

include_directories(
  ../../vtkVisuManagement
  ../../vtkRenderingAddOn
  )

if (APPLE)
set(GUI_EXECUTABLE MACOSX_BUNDLE)
endif()

ADD_EXECUTABLE(${PROJECT_NAME} 
  ${GUI_EXECUTABLE}
  SHManager.cxx
  )

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  vtkVisuManagement
  vtkRenderingAddOn
  )

