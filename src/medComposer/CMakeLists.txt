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

project(medComposer)


## #############################################################################
## find requireds
## #############################################################################

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})


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
  ${medCore_INCLUDE_DIRS}
  )


set(${PROJECT_NAME}_RCC medComposer.qrc)

qt5_add_resources(${PROJECT_NAME}_SOURCES_QRC ${${PROJECT_NAME}_RCC})

## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_CFILES}
    ${${PROJECT_NAME}_SOURCES_QRC}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}
    ${QT_LIBRARIES}
  medCore
  medCoreLegacy
  medWidgets
  dtkLog
  dtkComposer
  dtkCore
  )


## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )
