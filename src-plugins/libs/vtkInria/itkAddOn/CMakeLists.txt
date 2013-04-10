project(itkAddOn)

if (WIN32 AND NOT CYGWIN AND NOT MINGW)
  ADD_LIBRARY(getoptcompatlib STATIC
    getoptcompat/getopt.c
    getoptcompat/getopt_long.c
  )
endif()

ADD_EXECUTABLE(itkReshaper
itkReshaper.cxx
)

if (WIN32 AND NOT CYGWIN AND NOT MINGW)
 target_link_libraries(itkReshaper 
  itkIOAddOn 
  getoptcompatlib
 )
else()
 target_link_libraries(itkReshaper
  itkIOAddOn
 )
endif()

subdirs(
  Registration
  IO
)
