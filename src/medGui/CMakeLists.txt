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


add_subdirectory(viewContainers)
add_subdirectory(toolboxes)
add_subdirectory(settingsWidgets)
add_subdirectory(lookUpTables)
add_subdirectory(factories)
add_subdirectory(database)
add_subdirectory(commonWidgets)

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


set(${PROJECT_NAME}_QRC medGui.qrc)

## #############################################################################
## include directorie.
## #############################################################################

##  see cmake/module/list_header_directories_to_include.cmake
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
  ${${PROJECT_NAME}_H}
  ${${PROJECT_NAME}_CPP}
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

## see cmake/module/set_lib_install_rules.Cmake
set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )


