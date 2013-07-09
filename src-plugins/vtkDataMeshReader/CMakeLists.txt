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

project(vtkDataMeshReaderPlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION ${medInria_VERSION})

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(VTK)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include (${ITK_USE_FILE})


set(${PROJECT_NAME}_HEADERS_MOC
  vtkDataMeshReaderPlugin.h
  vtkDataMeshReader.h
  vtkDataMesh4DReader.h
  )

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  vtkDataMeshReaderPlugin.cpp
  vtkDataMeshReader.cpp
  vtkDataMesh4DReader.cpp
  )

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

if (MEDINRIA-PLUGINS_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH
    "medPluginsPch.h" "../libs/medPluginsPch/medPluginsVtkPch.cpp"
    ${${PROJECT_NAME}_SOURCES_MOC}
    ${${PROJECT_NAME}_SOURCES})
endif()

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${ITK_LIBRARIES}
  dtkVtkLogForwarder
  dtkCore dtkLog
  vtkCommon
  vtkFiltering
  vtkIO
  vtkDataManagement
  itksys
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


