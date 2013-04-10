PROJECT(PolydataToFiberXML)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkDataManagement
  )

add_execitable(${PROJECT_NAME}
  PolydataToFiberXML.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkDataManagement
  vtkRendering
  vtkIO
  )


