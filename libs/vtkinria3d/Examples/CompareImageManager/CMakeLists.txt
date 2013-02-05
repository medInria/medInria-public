PROJECT(CompareImageManager)

FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})

ADD_EXECUTABLE(CompareImageManager
CompareImageManager.cxx
)

TARGET_LINK_LIBRARIES(CompareImageManager
vtkRenderingAddOn
vtkVisuManagement
vtkRendering
vtkIO
)




