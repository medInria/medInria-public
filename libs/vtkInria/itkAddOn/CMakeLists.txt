project(itkAddOn)

IF(WIN32 AND NOT CYGWIN AND NOT MINGW)
  ADD_LIBRARY(getoptcompatlib STATIC
    getoptcompat/getopt.c
    getoptcompat/getopt_long.c
  )
ENDIF(WIN32 AND NOT CYGWIN AND NOT MINGW)

ADD_EXECUTABLE(itkReshaper
itkReshaper.cxx
)

IF(WIN32 AND NOT CYGWIN AND NOT MINGW)
 TARGET_LINK_LIBRARIES(itkReshaper 
  itkIOAddOn 
  getoptcompatlib
 )
ELSE(WIN32 AND NOT CYGWIN AND NOT MINGW)
 TARGET_LINK_LIBRARIES(itkReshaper
  itkIOAddOn
 )
ENDIF(WIN32 AND NOT CYGWIN AND NOT MINGW)

subdirs(
  IO
)
