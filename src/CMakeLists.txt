################################################################################
#
# medInria

# Copyright (c) INRIA. All rights reserved.

# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

cmake_minimum_required(VERSION 3.19)

set(QT_VERSION_MAJOR 5)

if(NOT DEFINED ${medInria_VERSION})
    set(medInria_VERSION 5.0.0)
endif()

include(CMakePackageConfigHelpers)

project(medInria VERSION ${medInria_VERSION})

cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0076 NEW)

SET(CMAKE_BUILD_PARALLEL_LEVEL 8 CACHE INTERNAL "" FORCE)

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)

set(${PROJECT_NAME}_IDENTIFIER fr.inria.${PROJECT_NAME})

add_compile_definitions(
  ${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}"
  ${PROJECT_NAME_UP}_IDENTIFIER="${${PROJECT_NAME}_IDENTIFIER}"
  )

set(medInria_ROOT ${CMAKE_BINARY_DIR})
set(PACKAGE_INIT "@PACKAGE_INIT@")
if(EXISTS ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in)
    configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in     "${CMAKE_BINARY_DIR}/lib/cmake/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake"         INSTALL_DESTINATION lib/cmake/${PROJECT_NAME})
                   CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}ModuleLocation.cmake ${CMAKE_BINARY_DIR}/lib/cmake/${PROJECT_NAME}/${PROJECT_NAME}ModuleLocation.cmake  COPYONLY)
endif()

set(${PROJECT_NAME_UP}_PATH ${CMAKE_BINARY_DIR})
## #############################################################################
## Macro
## #############################################################################

add_definitions( -DPROJECT_TEAM_NAME="medInria team" )
add_definitions( -DPROJECT_CONTACT="medinria-userfeedback@inria.fr" )

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

option(USE_OSPRay
  "Build with OSPRay support"
  OFF
  )

if (NOT WIN32)
option(USE_FFmpeg
  "Build with FFmpeg video export support"
  OFF
  )
endif()

## #############################################################################
## Find package
## #############################################################################

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
  Concurrent
  Core
  Gui
  Network
  OpenGL
  Quick
  Sql
  Svg
  Test
  Widgets
  Xml
  )

find_package(dtk REQUIRED)

## #############################################################################
## enable c++ 17
## #############################################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MACOSX_RPATH OFF)

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
include(set_lib_properties_variables)
include(set_plugin_install_rules)
include(add_external_resources)

include(add_plugins)
if(APPLE)
  set(CMAKE_MACOSX_RPATH OFF)
endif()



## #############################################################################
## Qt Auto tools
## #############################################################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)

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

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  message(STATUS "Add -DQT_NO_DEBUG")
  add_definitions(-DQT_NO_DEBUG)
endif()

## #############################################################################
## Windows specificity
## #############################################################################

if (WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd4068")
endif()

## #############################################################################
## Setup resources directory (for Windows and Linux)
## #############################################################################

if (NOT APPLE)
  set(${PROJECT_NAME}_RESOURCES_DIR "${CMAKE_BINARY_DIR}/resources")
  add_custom_target(make_resource_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${${PROJECT_NAME}_RESOURCES_DIR}"
    )
endif()

## #############################################################################
## Add subdirectory
## #############################################################################

add_subdirectory(layers)
add_subdirectory(app)
add_subdirectory(plugins)

if(${PROJECT_NAME}_BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif()

################################################################################
# Setup Startup project
################################################################################

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT medInria)

################################################################################
# Setup Configuration files
################################################################################

set(PACKAGE_INIT "@PACKAGE_INIT@")
if(EXISTS ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in)
    configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in     "${CMAKE_BINARY_DIR}/lib/cmake/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake"         INSTALL_DESTINATION lib/cmake/${PROJECT_NAME})
                   CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}ModuleLocation.cmake ${CMAKE_BINARY_DIR}/lib/cmake/${PROJECT_NAME}/${PROJECT_NAME}ModuleLocation.cmake  COPYONLY)
    INSTALL(FILES "${CMAKE_BINARY_DIR}/lib/cmake/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake"  DESTINATION          lib/cmake/${PROJECT_NAME}/)
    INSTALL(FILES  ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}ModuleLocation.cmake                        DESTINATION          lib/cmake/${PROJECT_NAME}/)
    INSTALL(DIRECTORY   ${CMAKE_SOURCE_DIR}/cmake/module                        DESTINATION          cmake/)
endif()


