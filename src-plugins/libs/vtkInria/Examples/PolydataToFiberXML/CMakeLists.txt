project(PolydataToFiberXML)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkDataManagement
  )

add_executable(${PROJECT_NAME}
  PolydataToFiberXML.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkDataManagement
  vtkRendering
  vtkIO
  )


