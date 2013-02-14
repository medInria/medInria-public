PROJECT(CompareImageManager)

include_directories(
../../vtkRenderingAddOn
../../vtkVisuManagement
)

ADD_EXECUTABLE(CompareImageManager
CompareImageManager.cxx
)

TARGET_LINK_LIBRARIES(CompareImageManager
vtkRenderingAddOn
vtkVisuManagement
vtkRendering
vtkIO
)




