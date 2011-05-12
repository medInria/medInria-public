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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(medinria-pluginsMacros)
find_package(dtk REQUIRED)
include(${dtk_USE_FILE})


## #################################################################
## Dashboard settings
## #################################################################

enable_testing()
OPTION(BUILD_EXAMPLES "Enable this if you want to build the exmples" OFF)

mark_as_advanced(DART_ROOT)

include(Dart)

## #################################################################
## Input
## #################################################################

set(DEPENDS_ON_DTK 1)

option(MEDINRIA_BUILD_TOOLS                               "Build Tools" "OFF")
option(MEDINRIA_BUILD_ALL_PLUGINS "Build all Medinria plugins (overriding selection)" OFF)


## ADD here any new plugin you want to include in the build.
set(PLUGIN_LIST v3dData v3dViewPublic v3dView itkDataImage itkDataImageReader itkDCMTKDataImageReader vtkDataMesh vtkDataMeshReader vtkDataMeshWriter itkDataDiffusionGradientList itkDataTensorImageWriter itkDataTensorImageReader )

if (MEDINRIA_BUILD_ALL_PLUGINS)
    set(PLUGIN_DEFAULT_BUILD_VALUE ON)
else()
    set(PLUGIN_DEFAULT_BUILD_VALUE OFF)
endif()

foreach (PLUGIN_ITEM  ${PLUGIN_LIST})
    MACRO_ADD_PLUGIN (${PLUGIN_ITEM} PLUGIN_DEFAULT_BUILD_VALUE)
endforeach()


## Registration plugins
option(MEDINRIA_ENABLE_REGISTRATION_PLUGINS "Build Registration plugins" "OFF")
if (MEDINRIA_ENABLE_REGISTRATION_PLUGINS OR MEDINRIA_BUILD_ALL_PLUGINS)
    set(REG_PLUGIN_LIST itkProcessRegistrationDiffeoMorphicDemons )
    foreach (PLUGIN_ITEM  ${REG_PLUGIN_LIST})
        MACRO_ADD_PLUGIN (${PLUGIN_ITEM} ON)
    endforeach()
#else()
#    foreach (PLUGIN_ITEM  ${REG_PLUGIN_LIST})
#        string(TOUPPER ${P_NAME} UPPER_CASE_NAME)
#        set (MEDINRIA_BUILD_${UPPER_CASE_NAME}_PLUGIN OFF)
#    endforeach()
endif()


if (MEDINRIA_BUILD_ALL_PLUGINS OR
    MEDINRIA_BUILD_V3DDATA_PLUGIN OR
    MEDINRIA_BUILD_V3DVIEWPUBLIC_PLUGIN OR
    MEDINRIA_BUILD_V3DVIEW_PLUGIN OR
    MEDINRIA_BUILD_VTKDATAMESH_PLUGIN OR
    MEDINRIA_BUILD_VTKDATAMESHREADER_PLUGIN OR
    MEDINRIA_BUILD_VTKDATAMESHWRITER_PLUGIN)
    set(MEDINRIA_REQUIRES_VTK TRUE)
endif()

if (MEDINRIA_BUILD_ALL_PLUGINS OR
    MEDINRIA_BUILD_ITKDATAIMAGE_PLUGIN OR
    MEDINRIA_BUILD_ITKDATAIMAGEREADER_PLUGIN OR
    MEDINRIA_BUILD_ITKDATAIMAGEWRITER_PLUGIN OR
    MEDINRIA_BUILD_ITKDATADIFFUSIONGRADIENTLIST_PLUGIN OR
    MEDINRIA_BUILD_ITKDATATENSORIMAGE_PLUGIN OR
    MEDINRIA_BUILD_ITKDATATENSORIMAGEREADER_PLUGIN OR
    MEDINRIA_BUILD_ITKDATATENSORIMAGEWRITER_PLUGIN OR
    MEDINRIA_BUILD_ITKDCMTKDATAIMAGEREADER_PLUGIN
    )
    set(MEDINRIA_REQUIRES_ITK TRUE)
endif()

add_subdirectory(src)


if (MEDINRIA_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

option(BUILD_DOCUMENTATION "Build documentation"    true)

if(BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif(BUILD_DOCUMENTATION)

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
