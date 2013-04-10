SUBDIRS(SynchronizedViews
        FibersManager
        IsosurfacesManager
        TensorManager
        CompareImageManager
        DataManager
        PolydataToFiberXML
        RegisterDataSets
  )

IF( VTKINRIA_USE_SPHERICALHARMONICS )
  add_subdirectory(SphericalHarmonicView)
endif()
