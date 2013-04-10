project(SHVisu)

include_directories(
  ../../vtkVisuManagement
  ../../vtkRenderingAddOn
  )

if (APPLE)
set(GUI_EXECUTABLE MACOSX_BUNDLE)
endif()

add_executable(${PROJECT_NAME} 
  ${GUI_EXECUTABLE}
  SHManager.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkVisuManagement
  vtkRenderingAddOn
  )

