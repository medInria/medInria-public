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
  add_library(itkIOAddOn STATIC
    ${itkIOAddOnSRCS}
  )
else()
  add_library( itkIOAddOn SHARED
    ${itkIOAddOnSRCS} 
  )
endif()

target_link_libraries(itkIOAddOn
  ${ITK_LIBRARIES}
)

if (ITK_USE_SYSTEM_GDCM)

set(SYSTEM_GDCM_LIBRARIES
  gdcmCommon
  gdcmDSED
  gdcmDICT
)

  add_executable(GDCMImport
    GDCMImport.cxx
    )
  if (WIN32 AND NOT CYGWIN AND NOT MINGW)
    target_link_libraries(GDCMImport 
      ${ITK_LIBRARIES}
      ${SYSTEM_GDCM_LIBRARIES}
      getoptcompatlib
      )
  else()
    target_link_libraries(GDCMImport
      ${ITK_LIBRARIES}
      ${SYSTEM_GDCM_LIBRARIES}
      )
  endif()
  
endif()


## #################################################################
## Install rules
## #################################################################
install(TARGETS itkIOAddOn
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME}) 
