### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Mon Sep 21 14:48:02 2009 (+0200)
## Version: $Id$
## Last-Updated: Wed Jan  6 10:51:50 2010 (+0100)
##           By: Julien Wintz
##     Update #: 15
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

project(medular-plugins)

mark_as_advanced(CMAKE_AR)

## #################################################################
## Setup output paths
## #################################################################

if(WIN32)
  set(${PROJECT_NAME}_ARCHIVE_OUTPUT_DIRECTORY lib)
  set(${PROJECT_NAME}_RUNTIME_OUTPUT_DIRECTORY bin)
  set(${PROJECT_NAME}_LIBRARY_OUTPUT_DIRECTORY bin)
else(WIN32)
  set(${PROJECT_NAME}_ARCHIVE_OUTPUT_DIRECTORY lib)
  set(${PROJECT_NAME}_RUNTIME_OUTPUT_DIRECTORY bin)
  set(${PROJECT_NAME}_LIBRARY_OUTPUT_DIRECTORY lib)
endif(WIN32)

set(CMAKE_DEBUG_POSTFIX "d")

set(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/${${PROJECT_NAME}_LIBRARY_OUTPUT_DIRECTORY})
set(ARCHIVE_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/${${PROJECT_NAME}_ARCHIVE_OUTPUT_DIRECTORY})
set(RUNTIME_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/${${PROJECT_NAME}_RUNTIME_OUTPUT_DIRECTORY})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${${PROJECT_NAME}_RUNTIME_OUTPUT_DIRECTORY})

set(${PROJECT_NAME}_LIBRARY_DIRS ${LIBRARY_OUTPUT_PATH})
set(${PROJECT_NAME}_RUNTIME_DIRS ${RUNTIME_OUTPUT_PATH})

## #################################################################
## Dashboard settings
## #################################################################

mark_as_advanced(DART_ROOT)

include(Dart)
enable_testing()

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH}) # for tests

## #################################################################
## Input
## #################################################################

subdirs(medItk)
subdirs(v3dData)
subdirs(v3dView)
subdirs(itkDataImage)
subdirs(itkDataImageReader)
subdirs(itkDataImageWriter)

