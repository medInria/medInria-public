### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Mon Sep 21 14:48:02 2009 (+0200)
## Version: $Id$
## Last-Updated: Thu Jun 17 09:42:14 2010 (+0200)
##           By: Julien Wintz
##     Update #: 25
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

cmake_minimum_required(VERSION 2.8.1)

project(medinria-plugins)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION_MAJOR 0 CACHE STRING "${PROJECT_NAME} major
version number.")
set(${PROJECT_NAME}_VERSION_MINOR 1 CACHE STRING "${PROJECT_NAME} minor
version number.")
set(${PROJECT_NAME}_VERSION_BUILD 0 CACHE STRING "${PROJECT_NAME} build
version number.")
set(${PROJECT_NAME}_VERSION
	  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)

## #################################################################
## Dashboard settings
## #################################################################

enable_testing()

mark_as_advanced(DART_ROOT)

include(Dart)

## #################################################################
## Input
## #################################################################

set(DEPENDS_ON_DTK 1)

option(MEDINRIA_BUILD_V3DDATA_PLUGIN                      "Build v3dData plugin" "ON")
option(MEDINRIA_BUILD_V3DVIEWPUBLIC_PLUGIN                "Build v3dViewPublic plugin" "ON")
option(MEDINRIA_BUILD_V3DVIEW_PLUGIN                      "Build v3dView plugin" "ON")
option(MEDINRIA_BUILD_ITKDATAIMAGE_PLUGIN                 "Build itkDataImge plugin" "ON")
option(MEDINRIA_BUILD_ITKDATAIMAGEREADER_PLUGIN           "Build itkDataImgeReader plugin" "ON")
option(MEDINRIA_BUILD_ITKDATAIMAGEWRITER_PLUGIN           "Build itkDataImgeWriter plugin" "ON")
option(MEDINRIA_BUILD_VTKDATAMESH_PLUGIN                  "Build vtkDataMesh plugin" "ON")
option(MEDINRIA_BUILD_VTKDATAMESHREADER_PLUGIN            "Build vtkDataMeshReader plugin" "ON")
option(MEDINRIA_BUILD_VTKDATAMESHWRITER_PLUGIN            "Build vtkDataMeshWriter plugin" "ON")
option(MEDINRIA_BUILD_ITKDATADIFFUSIONGRADIENTLIST_PLUGIN "Build itkDataDiffusionGradientList plugin" "ON")
option(MEDINRIA_BUILD_ITKDATATENSORIMAGE_PLUGIN           "Build itkDataTensorImage plugin" "ON")
option(MEDINRIA_BUILD_ITKDATATENSORIMAGEREADER_PLUGIN     "Build itkDataTensorImageReader plugin" "ON")

if (MEDINRIA_BUILD_V3DDATA_PLUGIN)
  subdirs(v3dData)
endif()

if (MEDINRIA_BUILD_V3DVIEWPUBLIC_PLUGIN)
  subdirs(v3dViewPublic)
endif()

if (MEDINRIA_BUILD_V3DVIEW_PLUGIN)
  subdirs(v3dView)
endif()

if (MEDINRIA_BUILD_ITKDATAIMAGE_PLUGIN)
  subdirs(itkDataImage)
endif()

if (MEDINRIA_BUILD_ITKDATAIMAGEREADER_PLUGIN)
  subdirs(itkDataImageReader)
endif()

if (MEDINRIA_BUILD_ITKDATAIMAGEWRITER_PLUGIN)
  subdirs(itkDataImageWriter)
endif()

if (MEDINRIA_BUILD_VTKDATAMESH_PLUGIN)
  subdirs(vtkDataMesh)
endif()

if (MEDINRIA_BUILD_VTKDATAMESHREADER_PLUGIN)
  subdirs(vtkDataMeshReader)
endif()

if (MEDINRIA_BUILD_VTKDATAMESHWRITER_PLUGIN)
  subdirs(vtkDataMeshWriter)
endif()

if (MEDINRIA_BUILD_ITKDATADIFFUSIONGRADIENTLIST_PLUGIN)
  subdirs(itkDataDiffusionGradientList)
endif()

if (MEDINRIA_BUILD_ITKDATATENSORIMAGE_PLUGIN)
  subdirs(itkDataTensorImage)
endif()

if (MEDINRIA_BUILD_ITKDATATENSORIMAGEREADER_PLUGIN)
    subdirs(itkDataTensorImageReader)
endif()

if (MEDINRIA_BUILD_V3DDATA_PLUGIN OR
    MEDINRIA_BUILD_V3DVIEWPUBLIC_PLUGIN OR
    MEDINRIA_BUILD_V3DVIEW_PLUGIN OR
    MEDINRIA_BUILD_VTKDATAMESH_PLUGIN OR
    MEDINRIA_BUILD_VTKDATAMESHREADER_PLUGIN OR
    MEDINRIA_BUILD_VTKDATAMESHWRITER_PLUGIN)
    
    set(MEDINRIA_REQUIRES_VTK TRUE)
endif()

if (MEDINRIA_BUILD_ITKDATAIMAGE_PLUGIN OR
    MEDINRIA_BUILD_ITKDATAIMAGEREADER_PLUGIN OR
    MEDINRIA_BUILD_ITKDATAIMAGEWRITER_PLUGIN OR
    MEDINRIA_BUILD_ITKDATADIFFUSIONGRADIENTLIST_PLUGIN OR
    MEDINRIA_BUILD_ITKDATATENSORIMAGE_PLUGIN OR
    MEDINRIA_BUILD_ITKDATATENSORIMAGEREADER_PLUGIN)
    
    set(MEDINRIA_REQUIRES_ITK TRUE)
endif()

if ( ${MEDINRIA_REQUIRES_VTK} )
    subdirs(dtkItkLogForwarder)
endif()

if ( ${MEDINRIA_REQUIRES_VTK} )
    subdirs(dtkVtkLogForwarder)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(pack)

## #################################################################
## Build settings
## #################################################################

# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security warnings
if(WIN32)
  if(NOT BORLAND)
    if(NOT CYGWIN)
      if(NOT MINGW)
        if(NOT ITK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)
          add_definitions(
            -D_CRT_FAR_MAPPINGS_NO_DEPRECATE
            -D_CRT_IS_WCTYPE_NO_DEPRECATE
            -D_CRT_MANAGED_FP_NO_DEPRECATE
            -D_CRT_NONSTDC_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE_GLOBALS
            -D_CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
            -D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
            -D_CRT_VCCLRIT_NO_DEPRECATE
            -D_SCL_SECURE_NO_DEPRECATE
            )
        endif(NOT ITK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)
      endif(NOT MINGW)
    endif(NOT CYGWIN)
  endif(NOT BORLAND)
endif(WIN32)
