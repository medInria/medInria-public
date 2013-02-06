PROJECT(PolydataToFiberXML)

FIND_PACKAGE(vtkInria REQUIRED)
INCLUDE(${vtkInria_USE_FILE})

ADD_EXECUTABLE(PolydataToFiberXML
PolydataToFiberXML.cxx
)

TARGET_LINK_LIBRARIES(PolydataToFiberXML
vtkDataManagement
vtkRendering
vtkIO
)


