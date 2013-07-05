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

cmake_minimum_required(VERSION 2.6)

project(itkDataImageWriterPlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION_MAJOR ${medInria_VERSION_MAJOR} CACHE STRING 
  "medInria major version number."
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)

set(${PROJECT_NAME}_VERSION_MINOR ${medInria_VERSION_MINOR} CACHE STRING 
  "medInria minor version number."
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)

set(${PROJECT_NAME}_VERSION_PATCH ${medInria_VERSION_PATCH} CACHE STRING 
  "medInria build version number."
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_PATCH)

set(${PROJECT_NAME}_VERSION_TWEAK ${medInria_VERSION_TWEAK} CACHE STRING 
  "medInria development marker."
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_TWEAK)

if (NOT ${${PROJECT_NAME}_VERSION_TWEAK} STREQUAL "")
  set(${PROJECT_NAME}_VERSION 
    ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}.${${PROJECT_NAME}_VERSION_TWEAK}
    )
else()
  set(${PROJECT_NAME}_VERSION
    ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}
    )
endif()

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  itkDataImageWriterPlugin.h)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  itkDataImageWriterPlugin.cpp
  itkMetaDataImageWriter.cpp
  itkAnalyzeDataImageWriter.cpp
  itkBMPDataImageWriter.cpp
  itkPNGDataImageWriter.cpp
  itkJPEGDataImageWriter.cpp
  itkTIFFDataImageWriter.cpp
  itkNiftiDataImageWriter.cpp
  itkNrrdDataImageWriter.cpp
  itkGiplDataImageWriter.cpp
  itkVTKDataImageWriter.cpp
  itkPhilipsRECDataImageWriter.cpp
  itkGISDataImageWriter.cpp)

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

if (MEDINRIA-PLUGINS_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH
    "medPluginsPch.h" "../libs/medPluginsPch/medPluginsItkPch.cpp"
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_SOURCES_MOC})
endif()

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkItkLogForwarder
  dtkCore dtkLog
  medCore
  ${ITK_LIBRARIES}
  ITKIOReview
  itkDataImageBase
  itkIOAddOn
  )

# Group common files together in Visual Studio.
source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib)
