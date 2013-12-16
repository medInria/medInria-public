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

project(medGui)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  viewContainers
  toolboxes
  settingsWidgets
  lookUpTables
  factories
  database
  commonWidgets
  )

set(${PROJECT_NAME}_QRC medGui.qrc)


## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )
  
include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${medCore_INCLUDE_DIRS}
  )
  

## #############################################################################
## Precompile headers
## #############################################################################

if (MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${MEDPCH_H}
    ${MEDPCH_CPP}
    ${${PROJECT_NAME}_CPP}
    )
endif()


## #############################################################################
## add library
## #############################################################################

qt4_add_resources(${PROJECT_NAME}_QRC 
  ${${PROJECT_NAME}_QRC}
  )

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_PCH}
  ${${PROJECT_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore
  dtkGui
  dtkLog
  medCore
  )


## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )


## #############################################################################
## Build tests
## #############################################################################

if(MEDINRIA_BUILD_TESTS)
  add_subdirectory(tests)
endif()
