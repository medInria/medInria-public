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

cmake_minimum_required(VERSION 2.8)

project(medInria)

set(${PROJECT_NAME}_VERSION_MAJOR 2 CACHE STRING "medInria major version number." FORCE)
set(${PROJECT_NAME}_VERSION_MINOR 0 CACHE STRING "medInria minor version number." FORCE)
set(${PROJECT_NAME}_VERSION_BUILD 2 CACHE STRING "medInria build version number." FORCE)
set(${PROJECT_NAME}_VERSION_BUGFIX 0 CACHE STRING "medInria bug-fix version number." FORCE)

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

add_definitions(-DMEDINRIA_VERSION="${${PROJECT_NAME}_VERSION}")

if (WIN32)
  option(MED_SHOW_CONSOLE "Show console at runtime (mainly for debugging)" "FALSE")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

option(MEDINRIA_BUILD_TESTING       "Build testing"          false)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(medInriaBuild)
include(medInria-pluginsBuild)

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

#configure ctest variables
option(MEDINRIA_BUILD_REPORTING "Build Reporting")
if (MEDINRIA_BUILD_REPORTING)
  include(dtkReporting)
endif()

if (MEDINRIA-PLUGINS_BUILD_TESTING)
  add_subdirectory(tests)
endif ()

## #################################################################
## Input
## #################################################################
if(MSVC)
  #Macros that enable precompiled headers to be added, defined by dtk.
  include(dtkPch)
  option(MEDINRIA-PLUGINS_USE_PRECOMPILED_HEADERS     "Use precompiled headers"        true)
endif()

set(DEPENDS_ON_DTK 1)

option(BUILD_ALL_PLUGINS "Build all Medinria plugins (overrides individual selections)" ON)

OPTION(MEDINRIA-PLUGINS_BUILD_EXAMPLES "Enable this if you want to build the examples" OFF)

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
  itkDataShImage                 ON
  itkDataShImageWriter           ON
  itkDataShImageReader           ON
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

if(BUILD_ALL_PLUGINS OR
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

if(BUILD_ALL_PLUGINS OR
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
  BUILD_ITKDATASHIMAGE_PLUGIN OR
  BUILD_ITKDATASHIMAGEREADER_PLUGIN OR
  BUILD_ITKDATASHIMAGEWRITER_PLUGIN OR
  BUILD_EXAMPLEPROCESSIMAGEFILTER_PLUGIN 
  )                   
  set(MEDINRIA_REQUIRES_ITK TRUE)
endif()

option(MEDINRIA_BUILD_DOCUMENTATION "Build documentation"    false)

if (MSVC)
  include(dtkPch)
  option(MEDINRIA_USE_PRECOMPILED_HEADERS     "Use precompiled headers"        false)
endif()

add_subdirectory(src)
add_subdirectory(src-plugins)
add_subdirectory(app)

if (MEDINRIA_BUILD_TESTING)
  add_subdirectory(tests)
endif()

if (MEDINRIA_BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif()
