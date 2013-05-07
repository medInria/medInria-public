SUBDIRS(SynchronizedViews
        FibersManager
        IsosurfacesManager
        TensorManager
        CompareImageManager
        DataManager
        PolydataToFiberXML
        RegisterDataSets
  )

if (VTKINRIA_USE_SPHERICALHARMONICS)
    add_subdirectory(SphericalHarmonicView)
endif()
