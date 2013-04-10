set(itkIOAddOnSRCS
  itkGISImageIO.cxx
  itkGISImageIOFactory.cxx
  itkMultiThreadedImageIOBase.cxx
)

if (NOT ITK_USE_SYSTEM_GDCM)
  set(itkIOAddOnSRCS
    ${itkIOAddOnSRCS}
    itkDicomTagManager.cxx
    itkGDCMImporter.cxx
    itkGDCMExporter.cxx
    )
endif()

if ( VTKINRIA_USE_SPHERICALHARMONICS )
  set(itkIOAddOnSRCS
    ${itkIOAddOnSRCS}
    itkSphericalHarmonicITKToVTKFilter.txx
    )
endif()

if ( WIN32 )
  ADD_LIBRARY(itkIOAddOn STATIC
    ${itkIOAddOnSRCS}
  )
else()
  ADD_LIBRARY( itkIOAddOn SHARED
    ${itkIOAddOnSRCS} 
  )
endif()

TARGET_LINK_LIBRARIES(itkIOAddOn
  ${ITK_LIBRARIES}
)

if (ITK_USE_SYSTEM_GDCM)

set(SYSTEM_GDCM_LIBRARIES
  gdcmCommon
  gdcmDSED
  gdcmDICT
)

  ADD_EXECUTABLE(GDCMImport
    GDCMImport.cxx
    )
  if (WIN32 AND NOT CYGWIN AND NOT MINGW)
    TARGET_LINK_LIBRARIES(GDCMImport 
      ${ITK_LIBRARIES}
      ${SYSTEM_GDCM_LIBRARIES}
      getoptcompatlib
      )
  else()
    TARGET_LINK_LIBRARIES(GDCMImport
      ${ITK_LIBRARIES}
      ${SYSTEM_GDCM_LIBRARIES}
      )
  endif()
  
endif()

