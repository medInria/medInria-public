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

set(TARGET_NAME medPacs)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

find_package(Qt5Widgets)

## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}

  ${${TARGET_NAME}_HEADERS}
  )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
    ${medCoreLegacy_INCLUDE_DIRS}
    )


## #############################################################################
## Add definition
## #############################################################################

if (NOT MSVC) #TODO : what is it for ?
  add_definitions(-Wno-write-strings)
  add_definitions(-Wformat=0)
endif()

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_SHARED)
add_definitions(-DQT_NO_DEBUG)

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  )

qt5_use_modules(${TARGET_NAME} Widgets)
qt5_use_modules(${TARGET_NAME} Core)
qt5_use_modules(${TARGET_NAME} Gui)

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
    ${QT_LIBRARIES}
    dtkCoreSupport
    dtkLog
    medCoreLegacy
    )
qt5_use_modules(${TARGET_NAME} Widgets)
qt5_use_modules(${TARGET_NAME} Core)
qt5_use_modules(${TARGET_NAME} Gui)

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )
