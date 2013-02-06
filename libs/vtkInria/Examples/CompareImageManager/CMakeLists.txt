PROJECT(CompareImageManager)

FIND_PACKAGE(vtkInria REQUIRED)
INCLUDE(${vtkInria_USE_FILE})

ADD_EXECUTABLE(CompareImageManager
CompareImageManager.cxx
)

TARGET_LINK_LIBRARIES(CompareImageManager
vtkRenderingAddOn
vtkVisuManagement
vtkRendering
vtkIO
)




