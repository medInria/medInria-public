PROJECT(PolydataToFiberXML)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkDataManagement
  )

ADD_EXECUTABLE(${PROJECT_NAME}
  PolydataToFiberXML.cxx
  )

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  vtkDataManagement
  vtkRendering
  vtkIO
  )


