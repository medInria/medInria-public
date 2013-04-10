PROJECT(FibersManager)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkVisuManagement
  )

if (APPLE)
  set(GUI_EXECUTABLE MACOSX_BUNDLE)
endif()

add_execitable(${PROJECT_NAME} 
  ${GUI_EXECUTABLE}
  FibersManager.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )


