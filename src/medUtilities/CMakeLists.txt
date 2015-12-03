################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

project(medUtilities)

## #############################################################################
## Find required
## #############################################################################

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
if (NOT VTK_USE_GLSL_SHADERS )
    message(FATAL_ERROR " VTK must be compiled with VTK_USE_GLSL_SHADERS at ON 
to use the HWShading library. This option is only available with VTK>=5.1."
    )
endif()

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

## #############################################################################
## List sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )


## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
${medVtkInria_INCLUDE_DIRS}
${medCore_INCLUDE_DIRS})

## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
${${PROJECT_NAME}_CFILES})

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}  
  ${OPENGL_LIBRARIES}
  medCore
  vtkCommon
  vtkFiltering 
  vtkRendering
  vtkWidgets
  vtkCommon
  vtkFiltering 
  vtkRendering
  vtkWidgets
  vtkHybrid
  vtkImaging
  vtkVolumeRendering
  vtkGraphics
  vtkInfovis
  vtkIO
  ITKVTK
  ITKCommon
  ITKIOPhilipsREC # ITK4 IO libs
  ITKIOBMP
  ITKIOBioRad
  ITKIOHDF5
  ITKIOGDCM
  ITKIOGIPL
  ITKIOGE
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


## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )
