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

project(medInria-plugins)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION_MAJOR 2 CACHE STRING "${PROJECT_NAME} major
version number." FORCE)
set(${PROJECT_NAME}_VERSION_MINOR 0 CACHE STRING "${PROJECT_NAME} minor
version number." FORCE)
set(${PROJECT_NAME}_VERSION_BUILD 2 CACHE STRING "${PROJECT_NAME} build
version number." FORCE)
set(${PROJECT_NAME}_VERSION_BUGFIX 0 CACHE STRING "${PROJECT_NAME} bug-fix version number." FORCE)

if (${${PROJECT_NAME}_VERSION_BUGFIX} GREATER 0)
  set(${PROJECT_NAME}_VERSION
    "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}-${${PROJECT_NAME}_VERSION_BUGFIX}")
else()
  set(${PROJECT_NAME}_VERSION
    "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")
endif()

mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUGFIX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#directories to include to the project
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeIncludeDirectories.cmake)

include(medInria-pluginsMacros)
find_package(dtk REQUIRED)
include(${dtk_USE_FILE})


#directories to include to the project
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeIncludeDirectories.cmake)

# Export medinriq-plugins_DIR variable so that internal project can automatically find it
set(${PROJECT_NAME}_DIR ${PROJECT_BINARY_DIR} CACHE INTERNAL "")


## #################################################################
## Dashboard settings
## #################################################################

#configure ctest variables
option(MEDINRIA-PLUGINS_BUILD_REPORTING "Build Reporting")
if (MEDINRIA-PLUGINS_BUILD_REPORTING)
  include(dtkReporting)
endif()

if (MEDINRIA-PLUGINS_BUILD_TESTING)
  add_subdirectory(tests)
endif()

## #################################################################
## Input
## #################################################################
if (MSVC)
  #Macros that enable precompiled headers to be added, defined by dtk.
  include(dtkPch)
  option(MEDINRIA-PLUGINS_USE_PRECOMPILED_HEADERS     "Use precompiled headers"        true)
endif()

set(DEPENDS_ON_DTK 1)

option(BUILD_ALL_PLUGINS "Build all Medinria plugins (overrides individual selections)" ON)

option(MEDINRIA-PLUGINS_BUILD_EXAMPLES "Enable this if you want to build the examples" OFF)

## ADD here any new plugin you want to include in the build, except for registration plugins, see below.
set(PLUGIN_LIST
  ${EXAMPLE_PLUGINS}
  v3dData                        ON
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
  medSegmentation                ON
  medCompositeDataSets           ON
  itkFilters                     ON
  qtdcmDataSource                ON
  )

if (MEDINRIA-PLUGINS_BUILD_EXAMPLES)
  set (PLUGIN_LIST
    ${PLUGIN_LIST}
    exampleProcessImageFilter      ON
    ITKProcessExample              ON
    )
endif()

#MACRO_ADD_PLUGIN_LIST ("${PLUGIN_LIST}")

list(LENGTH PLUGIN_LIST N)
set (INDEX 0)
#message(${PLUGIN_LIST})
while(INDEX LESS N)
  MATH( EXPR INDEX_BOOL "${INDEX} + 1" )
  #message("index:" ${INDEX})
  #message("index_bool:" ${INDEX_BOOL})

  list(GET PLUGIN_LIST ${INDEX} PLUGIN_ITEM)
  list(GET PLUGIN_LIST ${INDEX_BOOL} BUILD_VALUE)

  #message("Item: " ${PLUGIN_ITEM} "; value: " ${BUILD_VALUE})

  MACRO_ADD_PLUGIN (${PLUGIN_ITEM} ${BUILD_VALUE})
  MATH( EXPR INDEX "${INDEX} + 2" )
endwhile()


## Registration plugins, registration plugins are only built if this option is
## enabled or MEDINRIA_BUILD_ALL_PLUGINS is ON.
option(ENABLE_REGISTRATION_PLUGINS "Build Registration plugins" "ON")
if (ENABLE_REGISTRATION_PLUGINS OR MEDINRIA_BUILD_ALL_PLUGINS)
  #Put here your registration plugin directory/project name.
  set(REG_PLUGIN_LIST itkProcessRegistrationDiffeomorphicDemons ON)
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

if (BUILD_ALL_PLUGINS OR
  BUILD_VTKDATAMESHWRITER_PLUGIN OR
  BUILD_VTKDATAMESHREADER_PLUGIN OR
  BUILD_VTKDATAMESH_PLUGIN OR
  BUILD_V3DVIEW_PLUGIN OR
  BUILD_V3DDATA_PLUGIN OR
  BUILD_ITKDCMTKDATAIMAGEREADER_PLUGIN OR
  BUILD_EXAMPLEPROCESSIMAGEFILTER_PLUGIN 
  )
  set(MEDINRIA_REQUIRES_VTK TRUE)
endif()

if (BUILD_ALL_PLUGINS OR
  BUILD_VTKDATAMESHWRITER_PLUGIN OR
  BUILD_VTKDATAMESHREADER_PLUGIN OR
  BUILD_VTKDATAMESH_PLUGIN OR
  BUILD_V3DVIEW_PLUGIN OR
  BUILD_V3DDATA_PLUGIN OR
  BUILD_QTDCMDATASOURCE_PLUGIN OR
  BUILD_SEGMENTATION_PLUGIN OR
  BUILD_MEDCOMPOSITEDATASET_PLUGIN OR
  BUILD_ITKPROCESSEXAMPLE_PLUGIN OR
  BUILD_ITKFILTERS_PLUGIN OR
  BUILD_ITKDCMTKDATAIMAGEREADER_PLUGIN OR
  BUILD_ITKDATATENSORIMAGEWRITER_PLUGIN OR
  BUILD_ITKDATATENSORIMAGEREADER_PLUGIN OR
  BUILD_ITKDATATENSORIMAGE_PLUGIN OR
  BUILD_ITKDATAIMAGEWRITER_PLUGIN OR
  BUILD_ITKDATAIMAGEREADER_PLUGIN OR
  BUILD_ITKDATAIMAGE_PLUGIN OR
  BUILD_ITKDATADIFFUSIONGRADIENTLIST_PLUGIN OR
  BUILD_ITKDCMTKDATAIMAGEREADER_PLUGIN OR
  BUILD_EXAMPLEPROCESSIMAGEFILTER_PLUGIN 
  )                   
  set(MEDINRIA_REQUIRES_ITK TRUE)
endif()

if (BUILD_ALL_PLUGINS OR
  ENABLE_REGISTRATION_PLUGINS
  )
  set(MEDINRIA_REQUIERS_ITKPROCESSREGISTRATION TRUE)
endif()

if (BUILD_ALL_PLUGINS OR
  BUILD_ITKDATAIMAGEWRITER_PLUGIN OR
  BUILD_ITKDATAIMAGEREADER_PLUGIN
  )
  set(MEDINRIA_REQUIERS_ITKDATAIMAGEBASE TRUE)
endif()

if (BUILD_ALL_PLUGINS OR
  BUILD_ITKDCMTKDATAIMAGEREADER_PLUGIN
  )
  set(MEDINRIA_REQUIERS_ITKDCMTKIO TRUE)
endif()


if (BUILD_ALL_PLUGINS OR
  BUILD_V3DVIEW_PLUGIN OR
  BUILD_V3DDATA_PLUGIN     
  )
  set(MEDINRIA_REQUIRES_VTKINRIA TRUE)
endif()

## Needs MEDINRIA_REQUIRES_ITK and MEDINRIA_REQUIRES_VTK
## to be set before  being included.
add_subdirectory(libs)

option(MEDINRIA-PLUGINS_BUILD_TOOLS  "Build Tools" "ON")
if (MEDINRIA-PLUGINS_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

option(MEDINRIA-PLUGINS_BUILD_DOCUMENTATION "Build documentation"    OFF)
if (MEDINRIA-PLUGINS_BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif()


#already did in medinria-pluginsPack.cmake
#include(${PROJECT_NAME}Pack)

## #################################################################
## Mac packaging tools
## #################################################################

if (APPLE)
  configure_file(${PROJECT_SOURCE_DIR}/mac_package_scripts/mac_plugins_release_prepare.sh.in
                 ${PROJECT_BINARY_DIR}/mac_plugins_release_prepare.sh)
endif()

## #################################################################
## Build settings
## #################################################################

# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security warnings
if (WIN32)
  if (NOT BORLAND)
    if (NOT CYGWIN)
      if (NOT MINGW)
        if (NOT ITK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)
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
        endif()
      endif()
    endif()
  endif()
endif()
