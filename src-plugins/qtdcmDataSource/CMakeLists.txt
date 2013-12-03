################################################################################
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
################################################################################


project(qtdcmDataSourcePlugin)


## #############################################################################
## Setup version numbering
## #############################################################################

set(${PROJECT_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")


## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})


find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(QtDCM REQUIRED)
include(${QTDCM_USE_FILE})


## #############################################################################
## List Sources
## #############################################################################

file(GLOB HEADERS
  *.h
  )
set(${PROJECT_NAME}_H
  ${HEADERS}
  ${${PROJECT_NAME}_H}
  )

file(GLOB CPP
  *.cpp
  )
set(${PROJECT_NAME}_CPP
  ${CPP}
  ${${PROJECT_NAME}_CPP}
  )


## #############################################################################
## Configure
## #############################################################################

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qtdcmDataSourceDefinitions.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/qtdcmDataSourceDefinitions.h
  )


## #############################################################################
## include directorie.
## #############################################################################

##  see cmake/module/list_header_directories_to_include.cmake
list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  ${QTDCM_INCLUDE_DIR}
  ${QTDCM_DIR}
  )


## #############################################################################
## Precompile headers
## #############################################################################

if (MEDINRIA-PLUGINS_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${MEDPLUGINSPCH_H}
    ${MEDPLUGINSPCH_CPP}
    ${${PROJECT_NAME}_CPP}
    )
endif()


## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_PCH}
  ${${PROJECT_NAME}_H}
  ${${PROJECT_NAME}_CPP}
  )


## #############################################################################
## Link
## #############################################################################

if (WIN32)
  target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
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
  ${QTDCM_LIBS}
  ${DCMTK_LIBRARIES}
  dtkCore
  dtkLog
  medCore
  medGui
  )
endif()

## #############################################################################
## Install rules
## #############################################################################

## see cmake/module/set_plugin_install_rules.cmake
set_plugin_install_rules(${PROJECT_NAME})
