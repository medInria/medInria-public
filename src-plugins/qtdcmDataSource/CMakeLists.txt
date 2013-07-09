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

project(qtdcmDataSourcePlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION_MAJOR ${medInria_VERSION_MAJOR} CACHE STRING 
  "medInria major version number." FORCE
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)

set(${PROJECT_NAME}_VERSION_MINOR ${medInria_VERSION_MINOR} CACHE STRING 
  "medInria minor version number." FORCE
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)

set(${PROJECT_NAME}_VERSION_PATCH ${medInria_VERSION_PATCH} CACHE STRING 
  "medInria build version number." FORCE
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_PATCH)

set(${PROJECT_NAME}_VERSION_TWEAK ${medInria_VERSION_TWEAK} CACHE STRING 
  "medInria development marker." FORCE
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

find_package(ITK REQUIRED)
if (ITK_FOUND)
	include(${ITK_USE_FILE})
endif()

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(QtDCM REQUIRED)
if (QTDCM_FOUND)
  include(${QTDCM_USE_FILE})
endif()

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  qtdcmDataSourcePlugin.h
  qtdcmDataSourceSerieToolBox.h
  qtdcmDataSourceServersSettingsToolBox.h
  qtdcmDataSource.h)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  qtdcmDataSourcePlugin.cpp
  qtdcmDataSourceSerieToolBox.cpp
  qtdcmDataSourceServersSettingsToolBox.cpp
  qtdcmDataSource.cpp)

## #################################################################
## Build rules
## #################################################################

configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/qtdcmDataSourceDefinitions.h.in
      ${CMAKE_CURRENT_BINARY_DIR}/qtdcmDataSourceDefinitions.h
)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

include_directories(${QTDCM_INCLUDE_DIR} ${QTDCM_DIR})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_MOC} 
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_HEADERS})

if (WIN32)
  target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${ITK_LIBRARIES}
  ${QTDCM_LIBS}
  ${DCMTK_LIBRARIES}
  dtkCore
  dtkLog
  medCore
  medGui
  netapi32
  )
else()
  target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${ITK_LIBRARIES}
  ${QTDCM_LIBS}
  ${DCMTK_LIBRARIES}
  dtkCore
  dtkLog
  medCore
  medGui)
endif()

# Only doing a fix mac link since qtdcm should already have been compiled and therefore run the FixDCMTKMacInstall
if (APPLE)
  include(FixDCMTKMacLink)
  FixDCMTKMacLink(${PROJECT_NAME})
endif()

dtk_build_doc(qtdcmDataSourcePlugin)

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
  ARCHIVE DESTINATION plugins)
