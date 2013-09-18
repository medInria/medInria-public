#######################################################################
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
#######################################################################

cmake_minimum_required(VERSION 2.8)

project(medInria)

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

if (WIN32)
  option(MED_SHOW_CONSOLE "Show console at runtime (mainly for debugging)" "FALSE")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

option(MEDINRIA_BUILD_TESTING "Build testing" false)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(medInriaBuild)
include(medInria-pluginsBuild)

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

#configure ctest variables
option(MEDINRIA_BUILD_REPORTING "Build Reporting")
if (MEDINRIA_BUILD_REPORTING)
  include(dtkReporting)
endif()

option(MEDINRIA_BUILD_DOCUMENTATION "Build documentation" false)

if (MSVC)
  include(dtkPch)
  option(MEDINRIA_USE_PRECOMPILED_HEADERS "Use precompiled headers" false)
endif()

add_subdirectory(src)
add_subdirectory(src-plugins)
add_subdirectory(app)

if (MEDINRIA_BUILD_TESTING)
  add_subdirectory(tests)
endif()

if (MEDINRIA_BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif()



