### CMakeLists.txt ---
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Sep 16 17:14:33 2009 (+0200)
## Version: $Id$
## Last-Updated: Thu Jun 10 10:35:51 2010 (+0200)
##           By: Julien Wintz
##     Update #: 38
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

cmake_minimum_required(VERSION 2.6)

project(med)

set(${PROJECT_NAME}_VERSION_MAJOR 0 CACHE STRING "medular major version number.")
set(${PROJECT_NAME}_VERSION_MINOR 1 CACHE STRING "medular minor version number.")
set(${PROJECT_NAME}_VERSION_BUILD 0 CACHE STRING "medular build version number.")
set(${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)

enable_testing()

set(DEPENDS_ON_DTK 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

include(medPack)
include(medDart)

add_subdirectory(src)
add_subdirectory(app)
