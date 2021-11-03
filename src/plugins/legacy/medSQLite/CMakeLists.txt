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

enable_testing(true)
FIND_PACKAGE(GTest REQUIRED)
FIND_PACKAGE(Qt5Test REQUIRED)


## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
 )

## #############################################################################
## include directorie.
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
        Qt5::Sql
        medCore
        medCoreLegacy
  )


#enable_testing()
##include(GoogleTest)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#list_header_directories_to_include(${TARGET_NAME_TEST}
#        ${${TARGET_NAME}_HEADERS}
#        )
#
#include_directories(${${TARGET_NAME_TEST}_INCLUDE_DIRS}
#        ${MEDINRIA_INCLUDE_DIRS}
#        /usr/local/include
#        )
#
#
#list(APPEND ${TARGET_NAME_TEST}_SOURCE_FILES
#        medSQLite.txx
#        test/medSQliteTest.cpp
#        )
#
#find_library(GMOCK_LIBRARY
#        NAMES gmock
#        libgmock
#        libgmock.dylib)
#
#find_library(GMOCK_MAIN_LIBRARY
#        NAMES gmock_main
#        libgmock_main
#        libgmock_main.dylib)
#
#find_library(GTEST_LIBRARY
#        NAMES gtest
#        libgtest
#        libgtest.dylib)
#
#find_library(GTEST_MAIN_LIBRARY
#        NAMES gtest_main
#        libgtest_main
#        libgtest_main.dylib)
#
#
#add_executable(${TARGET_NAME_TEST} ${${TARGET_NAME_TEST}_SOURCE_FILES} test/medSQLiteRequestTest.cpp test/medSQLiteTest.cpp test/medSQLiteTest.h)
#
#target_link_libraries(${TARGET_NAME_TEST}
#        ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY}
#        ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY}
#        medCore
#        Qt5::Sql
#        ${TARGET_NAME})

set(TARGET_NAME_TEST ${TARGET_NAME}Test)

add_executable(${TARGET_NAME_TEST})
target_sources(${TARGET_NAME_TEST} PUBLIC medSQLite.txx test/medSQLiteRequestTest.cpp test/medSQLiteTest.cpp test/medSQLiteTest.h)
target_include_directories(${TARGET_NAME_TEST} PUBLIC "." "./test/")
set_target_properties(${TARGET_NAME_TEST} PROPERTIES COMPILE_DEFINITIONS "GTEST_LINKED_AS_SHARED_LIBRARY=1")
target_link_libraries(${TARGET_NAME_TEST} PUBLIC
		GTest::gtest_main
        GTest::gtest
		GTest::gmock_main
		GTest::gmock
		Qt5::Core
        Qt5::Sql
        medCore
        medCoreLegacy
        ${TARGET_NAME})



set(TARGET_NAME_QTEST ${TARGET_NAME}QTest)
add_executable(${TARGET_NAME_QTEST})
target_sources(${TARGET_NAME_QTEST} PUBLIC medSQLite.txx medSQLiteIntegrationQTest.cpp)
target_include_directories(${TARGET_NAME_QTEST} PUBLIC "." "./test/")
target_link_libraries(${TARGET_NAME_QTEST} PUBLIC
        Qt5::Test
        Qt5::Sql
        medCore)

#project(medSQLiteIntegrationQTest LANGUAGES CXX)
#
#find_package(Qt5Test REQUIRED)
#
##set(CMAKE_INCLUDE_CURRENT_DIR ON)
#
#set(CMAKE_AUTOMOC ON)
#
#enable_testing(true)
#
#add_executable(medSQLiteIntegrationQTest test/medSQLiteIntegrationQTest.cpp)
#add_test(NAME medSQLiteIntegrationQTest COMMAND medSQLiteIntegrationQTest)
#
#target_link_libraries(medSQLiteIntegrationQTest PRIVATE Qt5::Test Qt5::Sql medCore)

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
