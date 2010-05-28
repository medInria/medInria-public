### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Mon Sep 21 14:48:02 2009 (+0200)
## Version: $Id$
## Last-Updated: Wed Apr 28 18:18:03 2010 (+0200)
##           By: Julien Wintz
##     Update #: 23
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

project(medular-plugins)

## #################################################################
## Dashboard settings
## #################################################################

enable_testing()

mark_as_advanced(DART_ROOT)

include(Dart)

## #################################################################
## Input
## #################################################################

set(DTK_AGGREGATE_PLUGINS 1)
set(DEPENDS_ON_DTK 1)

subdirs(v3dData)
subdirs(v3dView)
subdirs(itkDataImage)
subdirs(itkDataImageReader)
subdirs(itkDataImageWriter)
subdirs(vtkDataFibers)
subdirs(itkDataDiffusionGradientList)

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
