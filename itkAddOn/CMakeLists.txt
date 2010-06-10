
IF(WIN32 AND NOT CYGWIN AND NOT MINGW)
  ADD_LIBRARY(getoptcompatlib STATIC
    getoptcompat/getopt.c
    getoptcompat/getopt_long.c
  )
ENDIF(WIN32 AND NOT CYGWIN AND NOT MINGW)


SET ( itkAddOnSRCS
  itkRegistrationFactory-explicit-template.cxx
  itkGISImageIO.cxx
  itkGISImageIOFactory.cxx
  itkMultiThreadedImageIOBase.cxx
)

IF (NOT ITK_USE_SYSTEM_GDCM)
  SET ( itkAddOnSRCS
    ${itkAddOnSRCS}
    itkDicomTagManager.cxx
    itkGDCMImporter.cxx
    itkGDCMExporter.cxx
    )
ENDIF(NOT ITK_USE_SYSTEM_GDCM)

IF( WIN32 )
  ADD_LIBRARY(itkAddOn STATIC
    ${itkAddOnSRCS}
  )
ELSE( WIN32 )
  ADD_LIBRARY( itkAddOn ${ITK_LIBS_STYLE}
    ${itkAddOnSRCS} 
  )
ENDIF( WIN32 )

TARGET_LINK_LIBRARIES(itkAddOn
  ${ITK_LIBRARIES}
)

ADD_EXECUTABLE(RegistrationFactory
RegistrationFactory.cxx
)

IF(WIN32 AND NOT CYGWIN AND NOT MINGW)
 TARGET_LINK_LIBRARIES(RegistrationFactory 
  itkAddOn 
  getoptcompatlib
 )
ELSE(WIN32 AND NOT CYGWIN AND NOT MINGW)
 TARGET_LINK_LIBRARIES(RegistrationFactory
  itkAddOn
 )
ENDIF(WIN32 AND NOT CYGWIN AND NOT MINGW)


ADD_EXECUTABLE(itkReshaper
itkReshaper.cxx
)

IF(WIN32 AND NOT CYGWIN AND NOT MINGW)
 TARGET_LINK_LIBRARIES(itkReshaper 
  itkAddOn 
  getoptcompatlib
 )
ELSE(WIN32 AND NOT CYGWIN AND NOT MINGW)
 TARGET_LINK_LIBRARIES(itkReshaper
  itkAddOn
 )
ENDIF(WIN32 AND NOT CYGWIN AND NOT MINGW)


#ADD_EXECUTABLE(transform_gradients
#transform_gradients.cxx
#)

#TARGET_LINK_LIBRARIES(transform_gradients
#  ${ITK_LIBRARIES}  
# )

IF (ITK_USE_SYSTEM_GDCM)

  ADD_EXECUTABLE(GDCMImport
    GDCMImport.cxx
    )
  IF(WIN32 AND NOT CYGWIN AND NOT MINGW)
    TARGET_LINK_LIBRARIES(GDCMImport 
      ${ITK_LIBRARIES}
      getoptcompatlib
      )
  ELSE(WIN32 AND NOT CYGWIN AND NOT MINGW)
    TARGET_LINK_LIBRARIES(GDCMImport
      ${ITK_LIBRARIES}
      )
  ENDIF(WIN32 AND NOT CYGWIN AND NOT MINGW)
  
ENDIF (ITK_USE_SYSTEM_GDCM)

if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
  install_targets(
    ${vtkINRIA3D_INSTALL_LIB_DIR}
    RUNTIME_DIRECTORY ${vtkINRIA3D_INSTALL_BIN_DIR}
    itkAddOn      
    )
endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

IF(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
  FILE(GLOB __files1 "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  FILE(GLOB __files2 "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")
  INSTALL(FILES ${__files1} ${__files2}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${vtkINRIA3D_INSTALL_INCLUDE_DIR})
ENDIF(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
