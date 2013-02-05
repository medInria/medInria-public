SUBDIRS(
SynchronizedViews
FibersManager
IsosurfacesManager
TensorManager
CompareImageManager
DataManager
PolydataToFiberXML
RegisterDataSets
)


IF( vtkINRIA3D_USE_SPHERICALHARMONICS )
add_subdirectory(
SphericalHarmonicView
)
ENDIF( vtkINRIA3D_USE_SPHERICALHARMONICS )
