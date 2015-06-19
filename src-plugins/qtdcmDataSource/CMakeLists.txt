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
include_directories(${dtk_INCLUDE_DIRS})


find_package(DCMTK REQUIRED)

find_package(QtDCM REQUIRED)
include(${QTDCM_USE_FILE})


## #############################################################################
## List Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  ${QTDCM_INCLUDE_DIR}
  ${QTDCM_DIR}
  )

## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_CFILES})

## #############################################################################
## Link
## #############################################################################

if (WIN32)
  set(WIN_LINK_LIBRARIES
	netapi32
	)
endif()

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${QTDCM_LIBS}
  ${DCMTK_LIBRARIES}
  ${WIN_LINK_LIBRARIES}
  dtkCore
  dtkLog
  medCore
  )
  
if(APPLE)
  FixDCMTKMacLink(${PROJECT_NAME} ${DCMTK_LIBRARIES})
endif(APPLE)


## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules(${PROJECT_NAME})
