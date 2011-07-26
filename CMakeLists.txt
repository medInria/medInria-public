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

project(medinria)

set(${PROJECT_NAME}_VERSION_MAJOR 0 CACHE STRING "medinria major version number.")
set(${PROJECT_NAME}_VERSION_MINOR 3 CACHE STRING "medinria minor version number.")
set(${PROJECT_NAME}_VERSION_BUILD 0 CACHE STRING "medinria build version number.")
set(${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)

if(WIN32)
  option(MED_SHOW_CONSOLE "Show console at runtime (mainly for debugging)" "FALSE")
endif()
  

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

include(medinriaBuild)
include(medinriaPack)
include(medinriaDart)

option(BUILD_DOCUMENTATION "Build documentation"    true)

add_subdirectory(src)
add_subdirectory(app)

if(BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif(BUILD_DOCUMENTATION)



if (APPLE)
  configure_file(${PROJECT_SOURCE_DIR}/mac_packager.sh.in
                 ${PROJECT_BINARY_DIR}/mac_packager.sh)
endif()
