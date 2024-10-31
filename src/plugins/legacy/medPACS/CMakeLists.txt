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

set(TARGET_NAME medPACSPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")


## #############################################################################
## Resolve dependencies
## #############################################################################

#enable_testing(true)
#FIND_PACKAGE(GTest REQUIRED)
#FIND_PACKAGE(Qt${QT_VERSION_MAJOR}Test REQUIRED)

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(DCMTK REQUIRED)

find_package(ITK REQUIRED)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  PACSOperations
  DICOMwebOperations
 )

## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
   ${${TARGET_NAME}_HEADERS}
   )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  ${DCMTK_INCLUDE_DIR}
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

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_PCH}
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################
target_link_libraries(${TARGET_NAME}
  medCore

  DCMTK::ofstd
  DCMTK::oflog
  DCMTK::dcmdata
  DCMTK::i2d
  DCMTK::dcmimgle
  DCMTK::dcmimage
  DCMTK::dcmjpeg
  DCMTK::ijg8
  DCMTK::ijg12
  DCMTK::ijg16
  DCMTK::dcmjpls
  DCMTK::dcmtkcharls
  DCMTK::dcmtls
  DCMTK::dcmnet
  DCMTK::dcmsr
  DCMTK::cmr
  DCMTK::dcmdsig
  DCMTK::dcmwlm
  DCMTK::dcmqrdb
  DCMTK::dcmpstat
  DCMTK::dcmrt
  DCMTK::dcmiod
  DCMTK::dcmfg
  DCMTK::dcmseg
  DCMTK::dcmtract
  DCMTK::dcmpmap
  DCMTK::dcmect
  )

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
