project(itkAddOn)

if (WIN32 AND NOT CYGWIN AND NOT MINGW)
  add_library(getoptcompatlib STATIC
    getoptcompat/getopt.c
    getoptcompat/getopt_long.c
  )
endif()

add_executable(itkReshaper
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
