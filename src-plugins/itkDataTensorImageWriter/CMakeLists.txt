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

project(itkDataTensorImageWriterPlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION_MAJOR 2 CACHE STRING "${PROJECT_NAME} major version number." FORCE)
set(${PROJECT_NAME}_VERSION_MINOR 0 CACHE STRING "${PROJECT_NAME} minor version number." FORCE)
set(${PROJECT_NAME}_VERSION_BUILD 0 CACHE STRING "${PROJECT_NAME} build version number." FORCE)
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

find_package(dtk REQUIRED)

include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(TTK REQUIRED)
include(${TTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  itkDataTensorImageWriterPlugin.h
  itkDataTensorImageWriterBase.h
  itkMetaDataTensorImageWriter.h
  itkNiftiDataTensorImageWriter.h
  itkNrrdDataTensorImageWriter.h)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  itkDataTensorImageWriterPlugin.cpp
  itkDataTensorImageWriterBase.cpp
  itkMetaDataTensorImageWriter.cpp
  itkNiftiDataTensorImageWriter.cpp
  itkNrrdDataTensorImageWriter.cpp)

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

if (MEDINRIA-PLUGINS_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH
    "medPluginsPch.h" "../libs/medPluginsPch/medPluginsItkPch.cpp"
    ${${PROJECT_NAME}_SOURCES_MOC}
    ${${PROJECT_NAME}_SOURCES})
endif()

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_HEADERS})

target_link_libraries(${PROJECT_NAME}
  ${ITK_LIBRARIES}
  ${QT_LIBRARIES}
  dtkCore dtkLog)

if (MEDINRIA-PLUGINS_BUILD_DOCUMENTATION)
	dtk_build_doc(${PROJECT_NAME})
endif()

## #################################################################
## Source file layout in development environments like Visual Studio
## #################################################################

source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib)
