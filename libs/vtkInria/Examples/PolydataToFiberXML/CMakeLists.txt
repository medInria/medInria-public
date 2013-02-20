PROJECT(PolydataToFiberXML)

include_directories(
../../vtkRenderingAddOn
../../vtkDataManagement
)

ADD_EXECUTABLE(PolydataToFiberXML
PolydataToFiberXML.cxx
)

TARGET_LINK_LIBRARIES(PolydataToFiberXML
vtkDataManagement
vtkRendering
vtkIO
)


