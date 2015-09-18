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

set(TARGET_NAME medInria)

## #############################################################################
## List Sources
## #############################################################################

add_subdirectory(QSingleApplication)
add_subdirectory(resources)

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

## #############################################################################
## include directorie.
## #############################################################################

##  see cmake/module/list_header_directories_to_include.cmake
list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${${PROJECT_NAME}_INCLUDE_DIRS}
  )

## #############################################################################
## Add preproc if we have revisions from the super project
## #############################################################################

if (EXISTS ${CMAKE_BINARY_DIR}/revisions/medRevisions.h)
  include_directories(${CMAKE_BINARY_DIR}/revisions)
  add_definitions(-D${PROJECT_NAME}_HAS_REVISIONS)
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

  set(${TARGET_NAME}_RC resources/medInria.rc)
endif()

## #############################################################################
## Add Exe
## #############################################################################

add_executable(${TARGET_NAME} ${DEPLOYMENT_SYSTEM} # Empty for Linux
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  ${${TARGET_NAME}_QSS} # Just to have it in EDI
  ${${TARGET_NAME}_RC}  # Empty for Unix
  )

if (APPLE)
  set_target_properties(${TARGET_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/MedInriaOSXBundleInfo.plist.in")
endif()

## #############################################################################
## Links.
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ${QT_LIBRARIES}
  dtkCoreSupport
  dtkGuiSupport
  dtkVrSupport
  medCore
  medCoreLegacy
  medPacs
  )

qt5_use_modules(${TARGET_NAME} Widgets)
qt5_use_modules(${TARGET_NAME} Core)
qt5_use_modules(${TARGET_NAME} Gui)

## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_exe_install_rules(${TARGET_NAME})
