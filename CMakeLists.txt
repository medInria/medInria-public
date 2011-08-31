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
set(${PROJECT_NAME}_VERSION_MINOR 4 CACHE STRING "${PROJECT_NAME} minor
version number.")
set(${PROJECT_NAME}_VERSION_BUILD 0 CACHE STRING "${PROJECT_NAME} build
version number.")
set(${PROJECT_NAME}_VERSION
	  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(medinria-pluginsMacros)
find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

#Macros that enable precompiled headers to be added, defined by dtk.
include(AddPch)

# Export medinriq-plugins_DIR variable so that internal project can automatically find i
set(medinria-plugins_DIR ${PROJECT_BINARY_DIR} CACHE INTERNAL "")

## #################################################################
## Dashboard settings
## #################################################################

enable_testing()
OPTION(BUILD_EXAMPLES "Enable this if you want to build the examples" OFF)

mark_as_advanced(DART_ROOT)

include(Dart)

## #################################################################
## Input
## #################################################################

set(DEPENDS_ON_DTK 1)

option(MEDINRIA_BUILD_ALL_PLUGINS "Build all Medinria plugins (overrides individual selections)" OFF)

## ADD here any new plugin you want to include in the build, except for registration plugins, see below.
set(PLUGIN_LIST
  v3dData                        ON 
  v3dViewPublic                  ON
  v3dView                        ON
  itkDataImage                   ON 
  itkDataImageReader             ON 
  itkDataImageWriter             ON
  itkDCMTKDataImageReader        ON
  vtkDataMesh                    ON
  vtkDataMeshReader              ON
  vtkDataMeshWriter              ON
  itkDataDiffusionGradientList   ON
  itkDataTensorImage             ON
  itkDataTensorImageWriter       ON
  itkDataTensorImageReader       ON
  ITKProcessExample              ON
  itkAddConstantToImageFilter    ON
  )

#MACRO_ADD_PLUGIN_LIST ("${PLUGIN_LIST}")

list(LENGTH PLUGIN_LIST N)
set (INDEX 0)
#message(${PLUGIN_LIST})
while(INDEX LESS N)
  MATH( EXPR INDEX_BOOL "${INDEX} + 1" )
  #MESSAGE ("index:" ${INDEX})
  #MESSAGE ("index_bool:" ${INDEX_BOOL})

  list(GET PLUGIN_LIST ${INDEX} PLUGIN_ITEM)
  list(GET PLUGIN_LIST ${INDEX_BOOL} BUILD_VALUE)

  #message("Item: " ${PLUGIN_ITEM} "; value: " ${BUILD_VALUE})

  MACRO_ADD_PLUGIN (${PLUGIN_ITEM} ${BUILD_VALUE})
  MATH( EXPR INDEX "${INDEX} + 2" )
endwhile()


## Registration plugins, registration plugins are only built if this option is
## enabled or MEDINRIA_BUILD_ALL_PLUGINS is ON.
option(MEDINRIA_ENABLE_REGISTRATION_PLUGINS "Build Registration plugins" "OFF")
if (MEDINRIA_ENABLE_REGISTRATION_PLUGINS OR MEDINRIA_BUILD_ALL_PLUGINS)

    #Put here your registration plugin directory/project name.
    set(REG_PLUGIN_LIST
      itkProcessRegistrationDiffeomorphicDemons OFF)

list(LENGTH REG_PLUGIN_LIST N)
set(INDEX 0)
while(INDEX LESS N)
  MATH( EXPR INDEX_BOOL "${INDEX} + 1" )

  list(GET REG_PLUGIN_LIST ${INDEX} PLUGIN_ITEM)
  list(GET REG_PLUGIN_LIST ${INDEX_BOOL} BUILD_VALUE)

  MACRO_ADD_PLUGIN (${PLUGIN_ITEM} ${BUILD_VALUE})
  #message(${PLUGIN_ITEM} " " ${BUILD_VALUE})

  MATH( EXPR INDEX "${INDEX} + 2" )
endwhile()

endif()

if (MEDINRIA_BUILD_EXAMPLEPROCESSIMAGEFILTER_PLUGIN)
    subdirs(exampleProcessImageFilter)
endif()

if (MEDINRIA_BUILD_ADDCONSTANTTOIMAGEFILTER_PLUGIN)
  add_subdirectory(itkAddConstantToImageFilter)
endif()

if(MSVC)
  option(MEDINRIA_PLUGINS_USE_PRECOMPILED_HEADERS     "Use precompiled headers"        true)
endif(MSVC)

if (MEDINRIA_BUILD_ALL_PLUGINS OR
    MEDINRIA_BUILD_V3DDATA_PLUGIN OR
    MEDINRIA_BUILD_V3DVIEWPUBLIC_PLUGIN OR
    MEDINRIA_BUILD_V3DVIEW_PLUGIN OR
    MEDINRIA_BUILD_VTKDATAMESH_PLUGIN OR
    MEDINRIA_BUILD_VTKDATAMESHREADER_PLUGIN OR
    MEDINRIA_BUILD_VTKDATAMESHWRITER_PLUGIN OR
    MEDINRIA_BUILD_EXAMPLEPROCESSIMAGEFILTER_PLUGIN)
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
    MEDINRIA_BUILD_ITKDCMTKDATAIMAGEREADER_PLUGIN OR
    MEDINRIA_BUILD_EXAMPLEPROCESSIMAGEFILTER_PLUGIN OR
    MEDINRIA_BUILD_EXAMPLEPROCESSIMAGEFILTER_PLUGIN OR
    MEDINRIA_BUILD_ITKPROCESSEXAMPLE_PLUGIN OR
    MEDINRIA_BUILD_ADDCONSTANTTOIMAGEFILTER_PLUGIN)
    set(MEDINRIA_REQUIRES_ITK TRUE)
endif()

## Needs MEDINRIA_REQUIRES_ITK and MEDINRIA_REQUIRES_VTK 
## to be set before  being included.
add_subdirectory(src)

option(MEDINRIA_BUILD_TOOLS  "Build Tools" "OFF")
if (MEDINRIA_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

option(BUILD_DOCUMENTATION "Build documentation"    true)
if(BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif(BUILD_DOCUMENTATION)

if (BUILD_TESTING)
  add_subdirectory(tests)
endif (BUILD_TESTING)

include(${PROJECT_NAME}Pack)

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
            -D_SCL_SECURE_NO_WARNINGS
          )
        endif(NOT ITK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)
      endif(NOT MINGW)
    endif(NOT CYGWIN)
  endif(NOT BORLAND)
endif(WIN32)