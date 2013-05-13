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

cmake_minimum_required(VERSION 2.8.1)

project(medSqlTests)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  medDataManagerTest.h)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  medDataManagerTest.cpp)

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_MAIN_SOURCES
  medDataManagerTest.cpp)

create_test_sourcelist(${PROJECT_NAME}_SOURCES_TEST ${PROJECT_NAME}.cxx
  ${${PROJECT_NAME}_MAIN_SOURCES})

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_SOURCES_TEST}
  ${${PROJECT_NAME}_SOURCES_MOC} 
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_HEADERS})

target_link_libraries(${PROJECT_NAME}
  ${QT_QTTEST_LIBRARY}
  ${QT_LIBRARIES}
  medGui
  medCore
  medTest
  medSql
  dtkCore)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")

foreach (test ${${PROJECT_NAME}_MAIN_SOURCES})
  get_filename_component(TName ${test} NAME_WE)
  add_test(NAME ${TName} COMMAND $<TARGET_FILE:${PROJECT_NAME}> ${TName})
endforeach()

## #################################################################
## Source file layout in development environments like Visual Studio
## #################################################################
SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})
