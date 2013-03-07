### CMakeLists.txt ---
##
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Sep 16 17:14:33 2009 (+0200)
## Version: $Id$
## Last-Updated: Tue May  3 16:59:29 2011 (+0200)
##           By: Julien Wintz
##     Update #: 39
######################################################################
##
### Commentary:
##
######################################################################
##
### Change log:
##
######################################################################

# Require 2.8 for correct behaviour of if( FALSE ).
cmake_minimum_required(VERSION 2.8)

project(medInria)

set(${PROJECT_NAME}_VERSION_MAJOR 2 CACHE STRING "medInria major version number." FORCE)
set(${PROJECT_NAME}_VERSION_MINOR 0 CACHE STRING "medInria minor version number." FORCE)
set(${PROJECT_NAME}_VERSION_BUILD 2 CACHE STRING "medInria build version number." FORCE)
set(${PROJECT_NAME}_VERSION_BUGFIX 0 CACHE STRING "medInria bug-fix version number." FORCE)

if (${${PROJECT_NAME}_VERSION_BUGFIX} GREATER 0)
  set(${PROJECT_NAME}_VERSION
    "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}-${${PROJECT_NAME}_VERSION_BUGFIX}")
else()
  set(${PROJECT_NAME}_VERSION
    "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")
endif()

mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUGFIX)

add_definitions(-DMEDINRIA_VERSION="${${PROJECT_NAME}_VERSION}")

if(WIN32)
  option(MED_SHOW_CONSOLE "Show console at runtime (mainly for debugging)" "FALSE")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

option(BUILD_TESTING       "Build testing"          true)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(medInriaBuild)
find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

include(medInriaPack)

#configure ctest variables
option(BUILD_REPORTING "Build Reporting")
if(BUILD_REPORTING)
  include(dtkReporting)
endif(BUILD_REPORTING)

option(BUILD_DOCUMENTATION "Build documentation"    true)

if(MSVC)
  include(dtkPch)
  option(MEDINRIA_USE_PRECOMPILED_HEADERS     "Use precompiled headers"        true)
endif(MSVC)

add_subdirectory(src)
add_subdirectory(app)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

if(BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif(BUILD_DOCUMENTATION)

if (APPLE)
  configure_file(${PROJECT_SOURCE_DIR}/mac_packager.sh.in
                 ${PROJECT_BINARY_DIR}/mac_packager.sh)
endif()
