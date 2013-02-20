PROJECT(IsosurfacesManager)

include_directories(
../../vtkRenderingAddOn
../../vtkVisuManagement
)

ADD_EXECUTABLE(IsosurfacesManager
IsosurfacesManager.cxx
)

TARGET_LINK_LIBRARIES(IsosurfacesManager
vtkRenderingAddOn
vtkVisuManagement
vtkRendering
vtkIO
)


