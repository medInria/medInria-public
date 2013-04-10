SET (itkIOAddOnSRCS
  itkGISImageIO.cxx
  itkGISImageIOFactory.cxx
  itkMultiThreadedImageIOBase.cxx
)

IF (NOT ITK_USE_SYSTEM_GDCM)
  SET (itkIOAddOnSRCS
    ${itkIOAddOnSRCS}
    itkDicomTagManager.cxx
    itkGDCMImporter.cxx
    itkGDCMExporter.cxx
    )
endif()

IF( VTKINRIA_USE_SPHERICALHARMONICS )
  SET (itkIOAddOnSRCS
    ${itkIOAddOnSRCS}
    itkSphericalHarmonicITKToVTKFilter.txx
    )
endif()

IF( WIN32 )
  ADD_LIBRARY(itkIOAddOn STATIC
    ${itkIOAddOnSRCS}
  )
ELSE( WIN32 )
  ADD_LIBRARY( itkIOAddOn SHARED
    ${itkIOAddOnSRCS} 
  )
endif()

TARGET_LINK_LIBRARIES(itkIOAddOn
  ${ITK_LIBRARIES}
)

IF (ITK_USE_SYSTEM_GDCM)

set(SYSTEM_GDCM_LIBRARIES
  gdcmCommon
  gdcmDSED
  gdcmDICT
)

  ADD_EXECUTABLE(GDCMImport
    GDCMImport.cxx
    )
  IF(WIN32 AND NOT CYGWIN AND NOT MINGW)
    TARGET_LINK_LIBRARIES(GDCMImport 
      ${ITK_LIBRARIES}
      ${SYSTEM_GDCM_LIBRARIES}
      getoptcompatlib
      )
  ELSE(WIN32 AND NOT CYGWIN AND NOT MINGW)
    TARGET_LINK_LIBRARIES(GDCMImport
      ${ITK_LIBRARIES}
      ${SYSTEM_GDCM_LIBRARIES}
      )
  endif()
  
endif()

