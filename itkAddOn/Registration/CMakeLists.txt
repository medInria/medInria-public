
set ( itkRegistrationAddOnSRCS
  itkRegistrationFactory-explicit-template.cxx
)

add_library ( itkRegistrationAddOn ${ITK_LIBS_STYLE}
  ${itkRegistrationAddOnSRCS} 
  )

target_link_libraries(itkRegistrationAddOn
  ${ITK_LIBRARIES}
  )

add_executable(RegistrationFactory
  RegistrationFactory.cxx
)

if(WIN32 AND NOT CYGWIN AND NOT MINGW)
 target_link_libraries(RegistrationFactory 
  itkRegistrationAddOn 
  getoptcompatlib
  )
else (WIN32 AND NOT CYGWIN AND NOT MINGW)
 target_link_libraries(RegistrationFactory
   itkRegistrationAddOn
   )
endif(WIN32 AND NOT CYGWIN AND NOT MINGW)

if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
  install(TARGETS itkRegistrationAddOn
    LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}     
    )
endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
  file(GLOB __files1 "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  file(GLOB __files2 "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")
  install(FILES ${__files1} ${__files2}
    DESTINATION ${vtkINRIA3D_INSTALL_INCLUDE_DIR})
endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
