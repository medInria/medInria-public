#######################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#######################################################################

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

SET(${PROJECT_NAME}_LIBRARIES
  itkIOAddOn
  ITKIOBMP
  ITKIOBioRad
  ITKIOHDF5
  ITKIOGDCM
  ITKIOGIPL
  ITKIOJPEG
  ITKIOLSM
  ITKIOMeta
  ITKIONIFTI
  ITKIONRRD
  ITKIOPNG
  ITKIOStimulate
  ITKIOVTK
  ITKIOMRC
)

if (WIN32 AND NOT CYGWIN AND NOT MINGW)
 target_link_libraries(itkReshaper 
  getoptcompatlib
  ${${PROJECT_NAME}_LIBRARIES}
 )
else()
 target_link_libraries(itkReshaper
  ${${PROJECT_NAME}_LIBRARIES}
 )
endif()

add_subdirectory(IO)
