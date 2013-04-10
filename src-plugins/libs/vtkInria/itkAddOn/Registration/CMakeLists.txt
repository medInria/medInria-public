set ( itkRegistrationAddOnSRCS
  itkRegistrationFactory-explicit-template.cxx
)

add_library (itkRegistrationAddOn SHARED
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
endif()


