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

project(medSqlTests)


## #############################################################################
## List Sources
## #############################################################################

file(GLOB_RECURSE ${PROJECT_NAME}_H
  *.h
  )

file(GLOB_RECURSE ${PROJECT_NAME}_CPP
  *.cpp
  )


## #############################################################################
## include directorie.
## #############################################################################

#see cmake/includeHeaderDirectories.cmake
list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
    ${medCore_INCLUDE_DIRS}
    ${medGui_INCLUDE_DIRS}
    )

## #############################################################################
## Qt stuff
## #############################################################################

# Find includes in corresponding source and build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
#set_property(PROPERTY AUTOMOC_TARGETS_FOLDER ${CMAKE_BINARY_DIR}/automoc} 
#TODO thing above may be needed for MSVC.


## #############################################################################
## Create test files
## #############################################################################

create_test_sourcelist(${PROJECT_NAME}_TEST ${PROJECT_NAME}.cxx
  ${PROJECT_NAME}_CPP
  )

## #############################################################################
## Add exe
## #############################################################################

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_TEST}
  ${${PROJECT_NAME}_CPP} 
  ${${PROJECT_NAME}_H}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_QTTEST_LIBRARY}
  ${QT_LIBRARIES}
  medGui
  medCore
  dtkCore
  )


## #############################################################################
## Runtime directory
## #############################################################################

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
  )


## #############################################################################
## Add tests
## #############################################################################

foreach (test ${${PROJECT_NAME}_CPP})
  get_filename_component(TName ${test} NAME_WE)
  add_test(NAME ${TName} COMMAND $<TARGET_FILE:${PROJECT_NAME}> ${TName})
endforeach()


