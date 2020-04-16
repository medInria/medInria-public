################################################################################
#
# medInria

# Copyright (c) INRIA 2013. All rights reserved.

# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################
set(TARGET_NAME qtdcmDataSourcePlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")


## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(DCMTK REQUIRED NO_MODULE)

find_package(QtDCM REQUIRED)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )


## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${TARGET_NAME}_PCH
    ${MEDPLUGINSPCH_H}
    ${MEDPLUGINSPCH_CPP}
    ${${TARGET_NAME}_SOURCES}
    )
endif()


## #############################################################################
## add library
## #############################################################################

link_directories(${QTDCM_LIBRARY_DIR})

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_PCH}
  ${${TARGET_NAME}_CFILES}
  )

target_include_directories(${TARGET_NAME}
  PUBLIC ${${TARGET_NAME}_INCLUDE_DIRS}
  PUBLIC  ${QTDCM_INCLUDE_DIRS}
  PUBLIC  ${QTDCM_DIR}
  PRIVATE ${MEDINRIA_INCLUDE_DIRS}
  )

## #############################################################################
## Link
## #############################################################################

if (WIN32)
  set(WIN_LINK_LIBRARIES
    netapi32
    )
endif()

target_link_libraries(${TARGET_NAME}
  ${QTDCM_LIBRARIES}
  ${DCMTK_LIBRARIES}
  ${WIN_LINK_LIBRARIES}
  dtkCore
  dtkLog
  medCore
  )


## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
