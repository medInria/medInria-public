################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

project(medVtkFibersDataPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${PROJECT_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")


## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(TTK REQUIRED)
include(${TTK_USE_FILE})

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  . #current dir
  datas
  interactors
  readers
  writers
  )


## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  )


## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${MEDPLUGINSPCH_H}
    ${MEDPLUGINSPCH_CPP}
    )
endif()


## #############################################################################
## add library
## #############################################################################

qt4_add_resources(${PROJECT_NAME}_QRC ${${PROJECT_NAME}_QRC})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_PCH}
  ${${PROJECT_NAME}_CFILES}
  ${${PROJECT_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  QVTK
  dtkCore
  dtkLog
  vtkCommon
  vtkFiltering
  vtkRendering
  medCore
  medLog
  medVtkInria
  ttkAlgorithms
  )


## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules(${PROJECT_NAME})
