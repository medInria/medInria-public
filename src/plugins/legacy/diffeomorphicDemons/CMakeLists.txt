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

set(TARGET_NAME diffeomorphicDemonsPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${${PROJECT_NAME}_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")

## #############################################################################
## Resolve dependencies
## #############################################################################
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick Xml Test)

if(NOT TARGET medCore)
  find_package(medInria REQUIRED COMPONENTS Core Registration)
endif()

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(RPI REQUIRED COMPONENTS common)

find_package(ITK REQUIRED COMPONENTS ITKCommon ITKIOTransformBase ITKIOTransformInsightLegacy ITKTransform ITKTransformFactory ITKRegistrationCommon ITKIOMeta ITKIOImageBase ITKPDEDeformableRegistration ITKIOTransformBase)
include(${ITK_USE_FILE})

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
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
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ${RPI_LIBRARIES}
  ${QT_LIBRARIES}
  dtkCore
  dtkLog
  ITKCommon
  ITKPDEDeformableRegistration
  med::Core
  med::Registration
  RPI::common
  )

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
