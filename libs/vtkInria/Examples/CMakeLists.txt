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


IF( vtkInria_USE_SPHERICALHARMONICS )
add_subdirectory(
SphericalHarmonicView
)
ENDIF( vtkInria_USE_SPHERICALHARMONICS )
