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

cmake_minimum_required(VERSION 2.8.9)

project(MEDINRIA)


## #############################################################################
## Set version
## #############################################################################

set(${PROJECT_NAME}_VERSION_MAJOR 2 CACHE STRING 
  "medInria major version number." 
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)

set(${PROJECT_NAME}_VERSION_MINOR 1 CACHE STRING 
  "medInria minor version number." 
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)

set(${PROJECT_NAME}_VERSION_PATCH 2 CACHE STRING 
  "medInria build version number." 
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_PATCH)

set(${PROJECT_NAME}_VERSION_TWEAK git CACHE STRING 
  "medInria development marker." 
  )
mark_as_advanced(${PROJECT_NAME}_VERSION_TWEAK)

if (NOT ${${PROJECT_NAME}_VERSION_TWEAK} STREQUAL "")
  set(${PROJECT_NAME}_VERSION 
    ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}.${${PROJECT_NAME}_VERSION_TWEAK}
    )
else()
  set(${PROJECT_NAME}_VERSION
    ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}
    )
endif()

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")


## #############################################################################
## Options 
## #############################################################################

option(${PROJECT_NAME}_BUILD_TESTS 
  "Build tests"
  OFF
  )
if(${PROJECT_NAME}_BUILD_TESTS)
    enable_testing()
endif()

option(${PROJECT_NAME}_BUILD_DOCUMENTATION 
  "Build documentation" 
  OFF
  )
  

## #############################################################################
## Additionals modules 
## #############################################################################

set(${PROJECT_NAME}_CMAKE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/module 
  )

set(CMAKE_MODULE_PATH 
  ${${PROJECT_NAME}_CMAKE_DIR}
  ${CMAKE_MODULE_PATH}
  )

include(list_source_files)
include(list_header_directories_to_include) 
include(set_exe_install_rules)
include(set_lib_install_rules)
include(set_plugin_install_rules)
include(add_plugins)
if(APPLE)
  include(FixDCMTKMacLink)
  include(FixDCMTKMacInstall)
endif()

## #############################################################################
## Find package
## ############################################################################# 

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})


find_package(dtk REQUIRED)
include(${dtk_USE_FILE})


## #############################################################################
## Qt Automoc
## #############################################################################

set(CMAKE_AUTOMOC ON)
#   Find includes in corresponding build (and source) directories
#   (nedded for generated files)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


## #############################################################################
## add definitions
## #############################################################################

if (CMAKE_CXX_COMPILER MATCHES clang)
    add_definitions(-DCLANG)
endif()

if (NOT MSVC) #TODO :why not this warings ?
  add_definitions(-Wno-write-strings)
  add_definitions(-Wformat=0)
endif()

add_definitions(${QT_DEFINITIONS}) #TODO :wath was it for ? (it is empty)
add_definitions(-DQT_SHARED)
add_definitions(-DQT_NO_DEBUG)


## #############################################################################
## Windows specificity
## #############################################################################

if (WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

if(MSVC)
  include(dtkPch)
  option(${PROJECT_NAME}_USE_PRECOMPILED_HEADERS "Use precompiled headers" 
    OFF
    )
endif()
if(${PROJECT_NAME}_USE_PRECOMPILED_HEADERS)
	message("WARNING precompiled headers are not functional so far.")
endif()

## #############################################################################
## Add subdirectory
## #############################################################################

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(src-plugins)

if(${PROJECT_NAME}_BUILD_DOCUMENTATION)
  add_subdirectory(documentation)
endif()


################################################################################
# Setup Configuration files
################################################################################

if(EXISTS ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in)
  configure_file( ## Build tree configure file
    ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    @ONLY IMMEDIATE
    )
endif()


################################################################################
# Setup use file
################################################################################

if(EXISTS ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Use.cmake.in)
  configure_file( ## Common use file
    ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Use.cmake.in
    ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake
    @ONLY IMMEDIATE
    )
endif()

if (APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MedInriaOSXBundleInfo.plist.in")
endif()




