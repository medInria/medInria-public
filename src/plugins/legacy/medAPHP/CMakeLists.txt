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

set(TARGET_NAME medAPHPPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")


## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(DCMTK REQUIRED NO_MODULE)

find_package(QtDCM REQUIRED)

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
  ${QTDCM_INCLUDE_DIRS}
  ${QTDCM_DIR}
  ${DCMTK_INCLUDE_DIR}
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
link_directories(${QTDCM_LIBRARY_DIR})

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_PCH}
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################
target_link_libraries(${TARGET_NAME}
  ${QTDCM_LIBRARIES}
  ${DCMTK_LIBRARIES}
  medCore
  )


## #############################################################################
## QTest
## #############################################################################
set(TARGET_NAME_TEST ${TARGET_NAME}QTest)

project(${TARGET_NAME_TEST} LANGUAGES CXX)

find_package(Qt5Test REQUIRED)

set(CMAKE_AUTOMOC ON)

enable_testing(true)

add_executable(${TARGET_NAME_TEST} tests/medAPHPQTest.cpp tests/downloader.cpp tests/downloader.h)
add_test(NAME ${TARGET_NAME_TEST} COMMAND ${TARGET_NAME_TEST})

target_link_libraries(${TARGET_NAME_TEST} PRIVATE
  Qt5::Test
  medCore
  ${TARGET_NAME}
  ${QTDCM_LIBRARIES}
  ${DCMTK_LIBRARIES}
)

### #############################################################################
### GTest
### #############################################################################

set(TARGET_NAME_GTEST ${TARGET_NAME}GTest)
project(${TARGET_NAME_GTEST} LANGUAGES CXX)
#
enable_testing()
include(GoogleTest)
find_package(GTest REQUIRED)
include_directories(
        ${MEDINRIA_INCLUDE_DIRS}
        /usr/local/include
)

find_library(GMOCK_LIBRARY
        NAMES gmock
        libgmock
        libgmock.dylib)

find_library(GMOCK_MAIN_LIBRARY
        NAMES gmock_main
        libgmock_main
        libgmock_main.dylib)

find_library(GTEST_LIBRARY
        NAMES gtest
        libgtest
        libgtest.dylib)

find_library(GTEST_MAIN_LIBRARY
        NAMES gtest_main
        libgtest_main
        libgtest_main.dylib)

add_executable(${TARGET_NAME_GTEST} tests/medAPHPGtest.cpp )

target_link_libraries(${TARGET_NAME_GTEST}
        ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY}
        ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY}
        medCore
        medCore
        ${TARGET_NAME}
        ${QTDCM_LIBRARIES}
        ${DCMTK_LIBRARIES}
)

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
