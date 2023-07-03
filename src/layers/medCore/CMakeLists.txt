################################################################################
#
# medInria

# Copyright (c) INRIA 2013. All rights reserved.

# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

set(TARGET_NAME medCore)

## #############################################################################
## find requireds
## #############################################################################

# find dtk
find_package(dtk REQUIRED)

find_package(Qt5Test REQUIRED)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  data
  source/operating
  source/settings
  source/threadPool
  source/virtualRepresentation
  source
  process
  process/arithmetic_operation
  process/diffusion_processes
  process/morphomath_operation
  process/mask_image
  process/single_filter
  parameter
  parameter/tools
  job
  notification
  log
  )


list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )


## #############################################################################
## include directories.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PRIVATE ${dtk_INCLUDE_DIRS}
  PUBLIC
	"$<BUILD_INTERFACE:${${TARGET_NAME}_INCLUDE_DIRS}>"
	"$<INSTALL_INTERFACE:include/${TARGET_NAME}>"
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  PUBLIC
  medCoreLegacy
  dtkCoreSupport
  PRIVATE
  Qt5::Core
  dtkGuiSupport
  dtkLog
  dtkComposer
  Qt5::Test
  )

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

#set_property(TARGET ${TARGET_NAME} PROPERTY VERSION ${medInria_VERSION})
##set_property(TARGET ${TARGET_NAME} PROPERTY SOVERSION 3)
##set_property(TARGET ${TARGET_NAME} PROPERTY INTERFACE_ClimbingStats_MAJOR_VERSION 3)
#
#include(GenerateExportHeader)
#generate_export_header(${TARGET_NAME})
#install(
#  FILES
#    ${${TARGET_NAME}_HEADERS}
#    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_export.h"
#  DESTINATION
#    include
#  COMPONENT
#    Devel
#)
#
#
#
#include(CMakePackageConfigHelpers)
#
#write_basic_package_version_file(
#  "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/${TARGET_NAME}ConfigVersion.cmake"
#  VERSION ${medInria_VERSION}
#  COMPATIBILITY AnyNewerVersion
#)
#
#export(EXPORT ${TARGET_NAME}Targets
#  FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/${TARGET_NAME}Targets.cmake"
#  NAMESPACE medInria::
#)
#
#configure_file(cmake/${TARGET_NAME}Config.cmake
#  "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/${TARGET_NAME}Config.cmake"
#  COPYONLY
#)

#configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
#  "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}Config.cmake"
#  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_NAME}
#)

#set(ConfigPackageLocation lib/cmake/${TARGET_NAME})
#install(EXPORT ${TARGET_NAME}Targets
#  FILE
#    ${TARGET_NAME}Targets.cmake
#  NAMESPACE
#    medInria::
#  DESTINATION
#    ${ConfigPackageLocation}
#)
#
#install(
#  FILES
#    cmake/${TARGET_NAME}Config.cmake
#    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/${TARGET_NAME}ConfigVersion.cmake"
#  DESTINATION
#    ${ConfigPackageLocation}
#  COMPONENT
#    Devel
#)


#install(EXPORT ${TARGET_NAME} DESTINATION ${CMAKE_BINARY_DIR}
#
#)

