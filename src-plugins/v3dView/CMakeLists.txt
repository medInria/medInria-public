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

cmake_minimum_required(VERSION 2.8.1)

project(v3dViewPlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION_MAJOR 2 CACHE STRING "${PROJECT_NAME} major version number." FORCE)
set(${PROJECT_NAME}_VERSION_MINOR 0 CACHE STRING "${PROJECT_NAME} minor version number." FORCE)
set(${PROJECT_NAME}_VERSION_BUILD 1 CACHE STRING "${PROJECT_NAME} build version number." FORCE)
set(${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

option(V3DVIEW_BUILD_TTK_DEPENDENT_CODE "Include TTK dependent code in ${PROJECT_NAME}" "ON")

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

if ( VTK_MAJOR_VERSION LESS 5 OR
     VTK_MINOR_VERSION LESS 8)
    message (SEND_ERROR "VTK 5.8.0 version is needed for the annotations in v3dView, please update to generate the v3dView project.")
endif()



if ( V3DVIEW_BUILD_TTK_DEPENDENT_CODE )
    # the Tensor ToolKit is required by the tensor interactor: v3dViewTensorInteractor
    find_package(TTK REQUIRED)
    include(${TTK_USE_FILE})
endif()

if (NOT VTK_USE_QT)
  message(WARNING "VTK has to be compiled with the option VTK_USE_QT:ON in order to compile this project. Please consider setting correctly your VTK.")
endif()

## #################################################################
## Input
## #################################################################

if ( V3DVIEW_BUILD_TTK_DEPENDENT_CODE )
    set(${PROJECT_NAME}_TTK_DEPENDENT_HEADERS_MOC
      v3dViewFiberInteractor.h
      v3dViewTensorInteractor.h
    )
    set(${PROJECT_NAME}_TTK_DEPENDENT_SOURCES
       v3dViewFiberInteractor.cpp
       v3dViewTensorInteractor.cpp
    )
    set(${PROJECT_NAME}_TTK_DEPENDENT_LINK_LIBRARIES
       ttkAlgorithms
    )
else()
    add_definitions(-DDISABLE_TTK_DEPENDENT_CODE)
endif()

set(${PROJECT_NAME}_HEADERS_MOC
  v3dViewPlugin.h
  v3dView.h
  v3dViewFuseInteractor.h
  v3dViewMeshInteractor.h
  v3dView4DInteractor.h
  v3dViewAnnotationInteractor.h
  ${${PROJECT_NAME}_TTK_DEPENDENT_HEADERS_MOC}
  v3dViewSHInteractor.h
  )

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC}
  annotations/vtkSeedPointHandle.h
  annotations/v3dViewAnnIntImageMaskHelper.h
  annotations/v3dViewAnnIntSeedPointHelper.h
  )

set(${PROJECT_NAME}_SOURCES
  v3dViewPlugin.cpp
  v3dView.cpp
  v3dViewFuseInteractor.cpp
  v3dViewMeshInteractor.cpp
  v3dView4DInteractor.cpp
  v3dFiberBundle.cpp
  v3dViewAnnotationInteractor.cpp
  annotations/v3dViewAnnIntImageMaskHelper.cpp
  annotations/v3dViewAnnIntSeedPointHelper.cpp
  annotations/vtkSeedPointHandle.cpp
  v3dViewSHInteractor.cpp
  ${${PROJECT_NAME}_TTK_DEPENDENT_SOURCES}
  )


## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

if (MEDINRIA_PLUGINS_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH
    "medPluginsPch.h" "../libs/medPluginsPch/medPluginsVtkPch.cpp"
    ${${PROJECT_NAME}_SOURCES_MOC}
    ${${PROJECT_NAME}_SOURCES})
endif()

include_directories(annotations)

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${ITK_LIBRARIES}
  ${${PROJECT_NAME}_TTK_DEPENDENT_LINK_LIBRARIES}
  dtkCore dtkLog
  medCore
  vtkCommon
  vtkRendering
  vtkImageView
  vtkWidgetsAddOn
  vtkVisuManagement
  vtkDataManagement
  vtkVolumeRendering
  QVTK
  vtkHWShading
  vtkHelpers)


if (MEDINRIA-PLUGINS_BUILD_TESTING)
  add_subdirectory(Testing)
endif()

# Include header files for Visual Studio.
source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib)
