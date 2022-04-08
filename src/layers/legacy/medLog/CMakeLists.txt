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

set(TARGET_NAME medLog)

## #############################################################################
## find requireds
## #############################################################################

# find dtk
find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

# find itk
find_package(ITK REQUIRED COMPONENTS ITKCommon)
include(${ITK_USE_FILE})

# find vtk
find_package(VTK REQUIRED COMPONENTS vtkCommonCore)
include(${VTK_USE_FILE})


## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )


## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  )


## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PUBLIC ${${TARGET_NAME}_INCLUDE_DIRS}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  Qt5::Core
  Qt5::Widgets
  dtkCoreSupport
  dtkLog
  ITKCommon
  vtkCommonCore
  )

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME} HEADERS
  ${${TARGET_NAME}_HEADERS}
  )
