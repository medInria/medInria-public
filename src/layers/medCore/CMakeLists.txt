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
## find required
## #############################################################################

# find dtk
find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

find_package(Qt5Test REQUIRED)
find_package(Qt5 REQUIRED Network)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  legacy
  legacy/data
  legacy/database
  legacy/data/annotationData
  legacy/gui
  legacy/gui/commonWidgets
  legacy/gui/database
  legacy/gui/factories
  legacy/gui/lookUpTables
  legacy/gui/settingsWidgets
  legacy/gui/toolboxes
  legacy/gui/viewContainers
  legacy/process
  legacy/views
  legacy/views/interactors
  legacy/views/navigators
  legacy/parameters
  legacy/resources
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
add_library(${TARGET_NAME} SHARED)
target_sources(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_CFILES} ${${TARGET_NAME}_QRC} )
target_sources(${TARGET_NAME} PRIVATE ${RELATIVE_${TARGET_NAME}_HEADERS})
set_target_properties(${TARGET_NAME} PROPERTIES PUBLIC_HEADER "${RELATIVE_${TARGET_NAME}_HEADERS}")

## #############################################################################
## include directories.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PRIVATE ${dtk_INCLUDE_DIRS}
  PUBLIC
	"$<BUILD_INTERFACE:${${TARGET_NAME}_INCLUDE_DIRS}>"
	"$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/${TARGET_NAME}>"
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  PUBLIC
  dtkCore
  dtkCoreSupport
  dtkGuiSupport

  Qt5::Core
  Qt5::Xml
  Qt5::Widgets
  Qt5::OpenGL
  Qt5::Gui
  Qt5::Sql
  Qt5::Test
  ${OPENGL_LIBRARIES}

  PRIVATE
  dtkLog
  Qt5::Network
  )
  
## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
    NAMESPACE                  med
    VERSION                    4.0    
    LIBRARY_OUTPUT_NAME_PREFIX med
    DESTINATION                ${MEDINRIA_PATH}
    
    PUBLIC_DEPENDENCIES        
      "dtk REQUIRED COMPONENTS dtkCore dtkCoreSupport dtkGuiSupport"
      "Qt5 REQUIRED COMPONENTS Core Xml Widgets OpenGL Gui Sql Test"
    )
