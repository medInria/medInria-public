PROJECT(IsosurfacesManager)

FIND_PACKAGE(vtkInria REQUIRED)
INCLUDE(${vtkInria_USE_FILE})

ADD_EXECUTABLE(IsosurfacesManager
IsosurfacesManager.cxx
)

TARGET_LINK_LIBRARIES(IsosurfacesManager
vtkRenderingAddOn
vtkVisuManagement
vtkRendering
vtkIO
)


