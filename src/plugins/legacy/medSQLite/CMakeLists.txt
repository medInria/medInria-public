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

set(TARGET_NAME medSQLitePlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")


## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(Qt${QT_VERSION_MAJOR} REQUIRED Core)

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})




## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/writingPolicy
 )

## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
   ${${TARGET_NAME}_HEADERS}
   )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  )


## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${TARGET_NAME}_PCH
    ${MEDPLUGINSPCH_H}
    ${MEDPLUGINSPCH_CPP}
    ${${TARGET_NAME}_SOURCES}
    )
endif()


## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_PCH}
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################
target_link_libraries(${TARGET_NAME} PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
        medCore
  )


#set(TARGET_NAME_TEST ${TARGET_NAME}Test)
#
#
#add_executable(${TARGET_NAME_TEST})
#target_sources(${TARGET_NAME_TEST} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/medSQLite.txx ${CMAKE_CURRENT_SOURCE_DIR}/test/medSQLiteRequestTest.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/test/medSQLiteTest.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/medSQLiteTest.h)
#target_include_directories(${TARGET_NAME_TEST} PUBLIC "." "./test/")
#set_target_properties(${TARGET_NAME_TEST} PROPERTIES COMPILE_DEFINITIONS "GTEST_LINKED_AS_SHARED_LIBRARY=1")
#target_link_libraries(${TARGET_NAME_TEST} PUBLIC
#		GTest::gtest_main
#        GTest::gtest
#		GTest::gmock_main
#		GTest::gmock
#		Qt${QT_VERSION_MAJOR}::Core
#        Qt${QT_VERSION_MAJOR}::Sql
#        medCore
#        ${TARGET_NAME})
#
#
#
#
#project(medSQLiteIntegrationQTest LANGUAGES CXX)
#
#find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
#
##set(CMAKE_INCLUDE_CURRENT_DIR ON)
#
#set(CMAKE_AUTOMOC ON)
#
#enable_testing(true)
#
#add_executable(medSQLiteIntegrationQTest ${CMAKE_CURRENT_SOURCE_DIR}/test/medSQLiteIntegrationQTest.cpp)
#add_test(NAME medSQLiteIntegrationQTest COMMAND medSQLiteIntegrationQTest)
#
#target_link_libraries(medSQLiteIntegrationQTest PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Sql medCore)

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
