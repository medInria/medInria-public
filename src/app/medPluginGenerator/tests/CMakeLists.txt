################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

project(medPluginGeneratorTest)


## #############################################################################
## Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )


## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  )


## #############################################################################
## Add definitions
## #############################################################################

add_definitions("-DCMAKE_DTK_ROOT=\"${dtk_ROOT}\""
  "-DCMAKE_ITK_ROOT=\"${ITK_ROOT}\""
  "-DCMAKE_RPI_ROOT=\"${RPI_ROOT}\""
  "-DCMAKE_BINARY_DIR=\"${CMAKE_BINARY_DIR}\""
  )

## #############################################################################
## Add Exe
## #############################################################################

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_CFILES}
  )


## #############################################################################
## Links.
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  medCore
  )


## #############################################################################
## Add test
## #############################################################################

add_test(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME})
