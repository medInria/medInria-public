### CMakeLists.txt ---
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Sep 16 17:14:33 2009 (+0200)
## Version: $Id$
## Last-Updated: Mon Sep 21 14:42:24 2009 (+0200)
##           By: Julien Wintz
##     Update #: 18
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

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION_MAJOR 0 CACHE STRING "medular major version number.")
set(${PROJECT_NAME}_VERSION_MINOR 1 CACHE STRING "medular minor version number.")
set(${PROJECT_NAME}_VERSION_BUILD 0 CACHE STRING "medular build version number.")
set(${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

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

set(${PROJECT_NAME}_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/src)

set(${PROJECT_NAME}_LIBRARY_DIRS ${LIBRARY_OUTPUT_PATH})
set(${PROJECT_NAME}_RUNTIME_DIRS ${RUNTIME_OUTPUT_PATH})

## #################################################################
## Setup CMake Modules
## #################################################################

set(${PROJECT_NAME}_CMAKE_DIR
  "${CMAKE_SOURCE_DIR}/cmake" CACHE INTERNAL "")

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} "${${PROJECT_NAME}_CMAKE_DIR}")

if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  install_files(${${PROJECT_NAME}_INSTALL_PACKAGE_DIR}/cmake "\\.cmake$")
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)

## #################################################################
## Setup installation settings
## #################################################################

set(${PROJECT_NAME}_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES})

if(NOT ${PROJECT_NAME}_INSTALL_BIN_DIR)
  set(${PROJECT_NAME}_INSTALL_BIN_DIR "/bin")
endif(NOT ${PROJECT_NAME}_INSTALL_BIN_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_LIB_DIR)
  set(${PROJECT_NAME}_INSTALL_LIB_DIR "/lib")
endif(NOT ${PROJECT_NAME}_INSTALL_LIB_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_DATA_DIR)
  set(${PROJECT_NAME}_INSTALL_DATA_DIR "/share" CACHE INTERNAL "")
endif(NOT ${PROJECT_NAME}_INSTALL_DATA_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_INCLUDE_DIR)
  set(${PROJECT_NAME}_INSTALL_INCLUDE_DIR "/include")
endif(NOT ${PROJECT_NAME}_INSTALL_INCLUDE_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_PACKAGE_DIR)
  set(${PROJECT_NAME}_INSTALL_PACKAGE_DIR ${${PROJECT_NAME}_INSTALL_LIB_DIR} CACHE INTERNAL "")
endif(NOT ${PROJECT_NAME}_INSTALL_PACKAGE_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  set(${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT 0)
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)

if(NOT ${PROJECT_NAME}_INSTALL_NO_RUNTIME)
  set(${PROJECT_NAME}_INSTALL_NO_RUNTIME 0)
endif(NOT ${PROJECT_NAME}_INSTALL_NO_RUNTIME)

if(NOT ${PROJECT_NAME}_INSTALL_NO_DOCUMENTATION)
  set(${PROJECT_NAME}_INSTALL_NO_DOCUMENTATION 0)
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DOCUMENTATION)

set(${PROJECT_NAME}_INSTALL_NO_LIBRARIES)
if(${PROJECT_NAME}_BUILD_SHARED_LIBS)
  if(${PROJECT_NAME}_INSTALL_NO_RUNTIME AND ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
    set(${PROJECT_NAME}_INSTALL_NO_LIBRARIES 1)
  endif(${PROJECT_NAME}_INSTALL_NO_RUNTIME AND ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
else(${PROJECT_NAME}_BUILD_SHARED_LIBS)
  if(${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
    set(${PROJECT_NAME}_INSTALL_NO_LIBRARIES 1)
  endif(${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
endif(${PROJECT_NAME}_BUILD_SHARED_LIBS)

## #################################################################
## Setup testing
## #################################################################

# include(CTest)

## #################################################################
## Resolve dependencies
## #################################################################

mark_as_advanced(CMAKE_BACKWARDS_COMPATIBILITY)
mark_as_advanced(CMAKE_BUILD_TYPE)
mark_as_advanced(CMAKE_INSTALL_PREFIX)
if(APPLE)
  mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
  mark_as_advanced(CMAKE_OSX_SYSROOT)
endif(APPLE)

set(QT_USE_QTOPENGL  TRUE)
set(QT_USE_QTSQL     TRUE)
set(QT_USE_QTHELP    TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTWEBKIT  TRUE)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

mark_as_advanced(QT_QMAKE_EXECUTABLE)

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

## #################################################################
## Build options
## #################################################################

option(BUILD_SHARED_LIBS "Build shared libraries" true)

## #################################################################
## Build rules
## #################################################################

include_directories(${${PROJECT_NAME}_INCLUDE_PATH})

add_subdirectory(src)
add_subdirectory(app)

## #################################################################
## Configuration rules for the build tree
## #################################################################

include(CMakeExportBuildSettings)

cmake_export_build_settings(
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}BuildSettings.cmake)

export_library_dependencies(
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}LibraryDepends.cmake)

if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  install_files(${${PROJECT_NAME}_INSTALL_PACKAGE_DIR} FILES
    ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake
    ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}LibraryDepends.cmake
    ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}BuildSettings.cmake
    )
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)

## Settings specific to the build tree.

set(${PROJECT_NAME}_CONFIG_INSTALL_ONLY)

set(${PROJECT_NAME}_USE_FILE_CONFIG
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake)

set(${PROJECT_NAME}_BUILD_SETTINGS_FILE_CONFIG 
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}BuildSettings.cmake)

set(${PROJECT_NAME}_LIBRARY_DIRS_CONFIG ${${PROJECT_NAME}_LIBRARY_DIRS})

set(${PROJECT_NAME}_RUNTIME_DIRS_CONFIG ${${PROJECT_NAME}_RUNTIME_DIRS})

set(${PROJECT_NAME}_INCLUDE_DIRS_CONFIG ${${PROJECT_NAME}_INCLUDE_PATH})

set(${PROJECT_NAME}_LIBRARY_DEPENDS_FILE 
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}LibraryDepends.cmake)

set(${PROJECT_NAME}_CMAKE_DIR_CONFIG ${${PROJECT_NAME}_CMAKE_DIR})

set(${PROJECT_NAME}_CONFIGURATION_TYPES_CONFIG ${${PROJECT_NAME}_CONFIGURATION_TYPES})

set(${PROJECT_NAME}_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  @ONLY IMMEDIATE)

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Use.cmake.in
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake
  @ONLY IMMEDIATE)

## Settings specific to the install tree.

set(DOLLAR "$")

set(${PROJECT_NAME}_USE_FILE_CONFIG 
  ${DOLLAR}{${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_PACKAGE_DIR}/${PROJECT_NAME}Use.cmake)

set(${PROJECT_NAME}_BUILD_SETTINGS_FILE_CONFIG 
  ${DOLLAR}{${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_PACKAGE_DIR}/${PROJECT_NAME}BuildSettings.cmake)

if(CYGWIN AND ${PROJECT_NAME}_BUILD_SHARED_LIBS)
  set(${PROJECT_NAME}_LIBRARY_DIRS_CONFIG 
    ${DOLLAR}{${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_BIN_DIR})
else(CYGWIN AND ${PROJECT_NAME}_BUILD_SHARED_LIBS)
  set(${PROJECT_NAME}_LIBRARY_DIRS_CONFIG 
    ${DOLLAR}{${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_LIB_DIR})
endif(CYGWIN AND ${PROJECT_NAME}_BUILD_SHARED_LIBS)

if(WIN32)
  set(${PROJECT_NAME}_RUNTIME_DIRS_CONFIG 
    ${DOLLAR}{${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_BIN_DIR})
else(WIN32)
  set(${PROJECT_NAME}_RUNTIME_DIRS_CONFIG 
    ${DOLLAR}{${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_LIB_DIR})
endif(WIN32)

set(${PROJECT_NAME}_INCLUDE_DIRS_CONFIG
  ${DOLLAR}{${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_INCLUDE_DIR})

set(${PROJECT_NAME}_LIBRARY_DEPENDS_FILE 
  ${DOLLAR}{${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_PACKAGE_DIR}/${PROJECT_NAME}LibraryDepends.cmake)

set(${PROJECT_NAME}_CMAKE_DIR_CONFIG 
  ${DOLLAR}{${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_PACKAGE_DIR}/CMake)

set(${PROJECT_NAME}_CONFIGURATION_TYPES_CONFIG)

string(REGEX REPLACE "/" ";" ${PROJECT_NAME}_INSTALL_PACKAGE_DIR_COUNT
  "${${PROJECT_NAME}_INSTALL_PACKAGE_DIR}")
set(${PROJECT_NAME}_CONFIG_INSTALL_ONLY "
# Compute the installation prefix from ${PROJECT_NAME}_DIR.
set(${PROJECT_NAME}_INSTALL_PREFIX \"${DOLLAR}{${PROJECT_NAME}_DIR}\")
")
foreach(p ${${PROJECT_NAME}_INSTALL_PACKAGE_DIR_COUNT})
  set(${PROJECT_NAME}_CONFIG_INSTALL_ONLY
    "${${PROJECT_NAME}_CONFIG_INSTALL_ONLY}GET_FILENAME_COMPONENT(${PROJECT_NAME}_INSTALL_PREFIX \"${DOLLAR}{${PROJECT_NAME}_INSTALL_PREFIX}\" PATH)\n"
    )
endforeach(p)

if(CMAKE_CONFIGURATION_TYPES)
  foreach(config ${CMAKE_CONFIGURATION_TYPES})
    set(${PROJECT_NAME}_BUILD_TYPE_CONFIG ${config})
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
      ${${PROJECT_NAME}_BINARY_DIR}/Install/${config}/${PROJECT_NAME}Config.cmake
      @ONLY IMMEDIATE)
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Use.cmake.in
      ${${PROJECT_NAME}_BINARY_DIR}/Install/${config}/${PROJECT_NAME}Use.cmake
      @ONLY IMMEDIATE)
  endforeach(config)

  if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
    install_files(${${PROJECT_NAME}_INSTALL_PACKAGE_DIR} FILES
      ${${PROJECT_NAME}_BINARY_DIR}/Install/${DOLLAR}{BUILD_TYPE}/${PROJECT_NAME}Config.cmake)
  endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
else(CMAKE_CONFIGURATION_TYPES)
  set(${PROJECT_NAME}_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${${PROJECT_NAME}_BINARY_DIR}/Install/${PROJECT_NAME}Config.cmake
    @ONLY IMMEDIATE)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Use.cmake.in
    ${${PROJECT_NAME}_BINARY_DIR}/Install/${PROJECT_NAME}Use.cmake
    @ONLY IMMEDIATE)

  if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
    install_files(${${PROJECT_NAME}_INSTALL_PACKAGE_DIR} FILES
      ${${PROJECT_NAME}_BINARY_DIR}/Install/${PROJECT_NAME}Config.cmake)
    install_files(${${PROJECT_NAME}_INSTALL_PACKAGE_DIR} FILES
      ${${PROJECT_NAME}_BINARY_DIR}/Install/${PROJECT_NAME}Use.cmake)
  endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
endif(CMAKE_CONFIGURATION_TYPES)

## #################################################################
## Uninstall target
## #################################################################

configure_file("${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Uninstall.cmake.in"
               "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake"
               IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake")
