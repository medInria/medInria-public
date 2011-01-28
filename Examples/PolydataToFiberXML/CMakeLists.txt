PROJECT(PolydataToFiberXML)

FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})

ADD_EXECUTABLE(PolydataToFiberXML
PolydataToFiberXML.cxx
)

TARGET_LINK_LIBRARIES(PolydataToFiberXML
vtkDataManagement
vtkRendering
vtkIO
)


