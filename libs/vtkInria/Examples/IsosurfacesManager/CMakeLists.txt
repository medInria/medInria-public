PROJECT(IsosurfacesManager)

FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})

ADD_EXECUTABLE(IsosurfacesManager
IsosurfacesManager.cxx
)

TARGET_LINK_LIBRARIES(IsosurfacesManager
vtkRenderingAddOn
vtkVisuManagement
vtkRendering
vtkIO
)


