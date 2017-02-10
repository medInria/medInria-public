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
project(MUSIC)

## #############################################################################
## List Sources
## #############################################################################

add_subdirectory(QSingleApplication)
add_subdirectory(resources)

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

## #############################################################################
## include directorie.
## #############################################################################

##  see cmake/module/list_header_directories_to_include.cmake
list_header_directories_to_include(${PROJECT_NAME} 
  ${${PROJECT_NAME}_HEADERS}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS} 
  ${MEDINRIA_INCLUDE_DIRS}
  )  

## #############################################################################
## Add preproc if we have revisions from the super project
## #############################################################################

if (EXISTS ${CMAKE_BINARY_DIR}/revisions/medRevisions.h)
  include_directories(${CMAKE_BINARY_DIR}/revisions)
  add_definitions(-DMEDINRIA_HAS_REVISIONS)
endif()


## #############################################################################
## Add preproc if we want immersive room stuff
## #############################################################################

option(ACTIVATE_WALL_OPTION 
  "Activate the command line option --wall (INRIA Sophia, immersive room)"
  OFF
  )
if (ACTIVATE_WALL_OPTION)
  add_definitions(-DACTIVATE_WALL_OPTION)
endif()

string(TIMESTAMP BUILD_DATE "\"%d_%m_%Y\"")
add_definitions(-DMEDINRIA_BUILD_DATE=${BUILD_DATE})

## #############################################################################
## OS specificites
## #############################################################################

if(WIN32)
  option(MED_SHOW_CONSOLE 
    "Show console at runtime (mainly for debugging)"
    OFF
    )
endif()


if (APPLE)
  set(DEPLOYMENT_SYSTEM MACOSX_BUNDLE)
endif()
if (WIN32)
  
  if(NOT ${MED_SHOW_CONSOLE} )
    set(DEPLOYMENT_SYSTEM WIN32)
  endif()
  
  set(${PROJECT_NAME}_RC resources/medInria.rc)
endif()

## #############################################################################
## Add Exe
## #############################################################################

qt4_add_resources(${PROJECT_NAME}_QRC 
  ${${PROJECT_NAME}_QRC}
  )

add_executable(${PROJECT_NAME} ${DEPLOYMENT_SYSTEM} # Empty for Linux
  ${${PROJECT_NAME}_CFILES}
  ${${PROJECT_NAME}_QRC}
  ${${PROJECT_NAME}_QSS} # Just to have it in EDI
  ${${PROJECT_NAME}_RC}  # Empty for Unix 
  )

if (APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/MedInriaOSXBundleInfo.plist.in")
endif()

## #############################################################################
## Links.
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore
  dtkScript
  dtkGui
  dtkVr
  medCore
  medPacs
  )


## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_exe_install_rules(${PROJECT_NAME})
