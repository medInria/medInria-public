################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
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

find_package(Boost REQUIRED)

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  areas/browser
  areas/composer
  areas/homepage
  areas/workspaces
  areas/workspaces/diffusion
  areas/workspaces/filtering
  areas/workspaces/generic
  areas/workspaces/registration
  areas/workspaces/segmentation
  areas/workspaces/visualization
  )

##  see cmake/module/list_header_directories_to_include.cmake
list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  ${Boost_INCLUDE_DIR}
  )
  
include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR}
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
## Variable
## #############################################################################

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
  ${${TARGET_NAME}_HTML}
  )

if (APPLE)
  set_target_properties(${TARGET_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/MedInriaOSXBundleInfo.plist.in")
endif()

## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PRIVATE ${${TARGET_NAME}_INCLUDE_DIRS}
  )
# PRIVATE beacause application is not supposed to be used somewhere else

## #############################################################################
## Links.
## #############################################################################

target_link_libraries(${TARGET_NAME}
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  Qt5::OpenGL
  dtkCoreSupport
  dtkGuiSupport
  dtkVrSupport
  dtkComposer
  medCore
  medComposer
  medWidgets
  medCoreLegacy
  medPacs
  )


## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_exe_install_rules(${TARGET_NAME})
