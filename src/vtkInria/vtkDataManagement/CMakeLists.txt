################################################################################
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
################################################################################

project(vtkDataManagement)


## #############################################################################
## List Sources
## #############################################################################

file(GLOB HEADERS
  *.h
  )
set(${PROJECT_NAME}_H
  ${HEADERS}
  ${${PROJECT_NAME}_H}
  )

file(GLOB CXX
  *.cxx
  )
set(${PROJECT_NAME}_CXX
  ${CXX}
  ${${PROJECT_NAME}_CXX}
  )

set(${PROJECT_NAME}_LIBS
  ITKCommon
  vtkCommon
  vtkRendering
  vtkWidgets
  vtkVolumeRendering
  vtkHybrid
  vtkImaging    
  vtkGraphics
  vtkFiltering
  vtkImaging
  vtkIO
  vtkRendering
  ITKVTK
  # ITK4 IO libs
  ITKIOPhilipsREC
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
  ${ITKReview_LIBRARIES} #TODO : get rid of this ASAP
  )

## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${VTKINRIAPCH_H}
    ${VTKINRIAPCH_CPP}
    ${${PROJECT_NAME}_CPP}
    )
endif()


## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_CXX}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}  
  ${${PROJECT_NAME}_LIBS}
  )


## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  ) 
